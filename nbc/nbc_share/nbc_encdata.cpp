#include <vector>
#include <nbc_share/nbc_utility.hpp>
#include <nbc_share/nbc_encdata.hpp>

#include <helib/FHE.h>
#include <helib/EncryptedArray.h>

namespace nbc_share
{

struct EncData::Impl
{
    Impl() : size_()
    {
    }

    void generate(const std::vector<long>& inputdata,
                  const std::string& pubkey_filename)
    {
        //unsigned long m, p, r;
        //std::vector<long> gens, ords;
        //std::ifstream ct(pubkey_filename);
        //helib::readContextBase(ct, m, p, r, gens, ords);
        //helib::FHEcontext context(m, p, r, gens, ords);
        //ct >> context;
        //helib::FHEPubKey publicKey(context);
        //ct >> publicKey;
        //ct.close();
        std::shared_ptr<helib::FHEcontext> context_ptr;
        std::shared_ptr<helib::FHEPubKey>  pubkey_ptr;
        read_context_and_pubkey(pubkey_filename,
                                context_ptr, pubkey_ptr);
        auto& context = *context_ptr;
        auto& pubkey  = *pubkey_ptr;

        NTL::ZZX G = context.alMod.getFactorsOverZZ()[0];
        helib::EncryptedArray ea(context, G);

        std::shared_ptr<helib::Ctxt> encdata_ptr(new helib::Ctxt(pubkey));
        auto& ctxt = *encdata_ptr;
        ea.encrypt(ctxt, pubkey, inputdata);

        ctxt_ptr_ = encdata_ptr;
        size_     = inputdata.size();
    }

    void save_to_stream(std::ostream& os) const
    {
        if (ctxt_ptr_)
        {
            os << size_ << std::endl;

            auto& ctxt = *ctxt_ptr_;
            os << ctxt << std::endl;
        }
    }

    void load_from_stream(std::istream& is, const std::string& pubkey_filename)
    {
        is >> size_;

        //unsigned long m, p, r;
        //std::vector<long> gens, ords;
        //std::ifstream ct(pubkey_filename);
        //::readContextBase(ct, m, p, r, gens, ords);
        //::FHEcontext context(m, p, r, gens, ords);
        //ct >> context;
        //::FHEPubKey publicKey(context);
        //ct >> publicKey;
        //ct.close();
        std::shared_ptr<helib::FHEcontext> context_ptr;
        std::shared_ptr<helib::FHEPubKey>  pubkey_ptr;
        read_context_and_pubkey(pubkey_filename,
                                context_ptr, pubkey_ptr);
        auto& pubkey  = *pubkey_ptr;

        std::shared_ptr<helib::Ctxt> ctxt_ptr(new helib::Ctxt(pubkey));
        auto& ctxt = *ctxt_ptr;
        is >> ctxt;

        ctxt_ptr_ = ctxt_ptr;
    }

    size_t size(void) const
    {
        return size_;
    }

    const helib::Ctxt& data(void) const
    {
        return *ctxt_ptr_;
    }

    helib::Ctxt& data(void)
    {
        return *ctxt_ptr_;
    }

    size_t stream_size(void) const
    {
        std::ostringstream oss;
        save_to_stream(oss);
        return oss.str().size();
    }
    
private:
    void read_context_and_pubkey(const std::string& pubkey_filename,
                                 std::shared_ptr<helib::FHEcontext>& context,
                                 std::shared_ptr<helib::FHEPubKey>& pubkey)
    {
        unsigned long m, p, r;
        std::vector<long> gens, ords;
        std::ifstream ct(pubkey_filename);
        helib::readContextBase(ct, m, p, r, gens, ords);
        
        std::shared_ptr<helib::FHEcontext> context_ptr(
            new helib::FHEcontext(m, p, r, gens, ords));
        ct >> *context_ptr;
        
        std::shared_ptr<helib::FHEPubKey> pubkey_ptr(
            new helib::FHEPubKey(*context_ptr));
        ct >> *pubkey_ptr;
        
        ct.close();

        context = context_ptr;
        pubkey  = pubkey_ptr;
    }

    
private:
    std::shared_ptr<helib::Ctxt> ctxt_ptr_;
    size_t size_;
};

EncData::EncData(void) : pimpl_(new Impl())
{
}

void EncData::generate(const std::vector<long>& inputdata,
                       const std::string& pubkey_filename)
{
    pimpl_->generate(inputdata, pubkey_filename);
}

void EncData::save_to_stream(std::ostream& os) const
{
    pimpl_->save_to_stream(os);
}

void EncData::load_from_stream(std::istream& is,
                                     const std::string& pubkey_filename)
{
    pimpl_->load_from_stream(is, pubkey_filename);
}

size_t EncData::size(void) const
{
    return pimpl_->size();
}

const helib::Ctxt& EncData::data(void) const
{
    return pimpl_->data();
}

helib::Ctxt& EncData::data(void)
{
    return pimpl_->data();
}

size_t EncData::stream_size(void) const
{
    return pimpl_->stream_size();
}

} /* namespace nbc_share */
