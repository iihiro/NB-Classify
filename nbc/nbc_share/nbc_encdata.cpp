#include <vector>
#include <stdsc/stdsc_exception.hpp>
#include <nbc_share/nbc_utility.hpp>
#include <nbc_share/nbc_encdata.hpp>
#include <nbc_share/nbc_pubkey.hpp>
#include <nbc_share/nbc_context.hpp>

#include <helib/FHE.h>
#include <helib/EncryptedArray.h>

namespace nbc_share
{

struct EncData::Impl
{
    Impl(const PubKey& pubkey)
        : pubkey_(pubkey),
          size_()
    {
    }

    void generate(const std::vector<long>& inputdata,
                  const Context& context)
    {
        auto& context_data = context.get();
        auto& pubkey_data  = pubkey_.get();

        NTL::ZZX G = context_data.alMod.getFactorsOverZZ()[0];
        helib::EncryptedArray ea(context_data, G);
        
        std::shared_ptr<helib::Ctxt> encdata_ptr(new helib::Ctxt(pubkey_data));
        auto& ctxt = *encdata_ptr;
        ea.encrypt(ctxt, pubkey_data, inputdata);
        
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

    void load_from_stream(std::istream& is)
    {
        is >> size_;

        auto& pubkey  = pubkey_.get();
        std::shared_ptr<helib::Ctxt> ctxt_ptr(new helib::Ctxt(pubkey));
        auto& ctxt = *ctxt_ptr;
        is >> ctxt;

        ctxt_ptr_ = ctxt_ptr;
    }

    void save_to_file(const std::string& filepath) const
    {
        std::ofstream ofs(filepath, std::ios::binary);
        save_to_stream(ofs);
        ofs.close();
    }
    
    void load_from_file(const std::string& filepath)
    {
        if (!nbc_share::utility::file_exist(filepath)) {
            std::ostringstream oss;
            oss << "File not found. (" << filepath << ")";
            STDSC_THROW_FILE(oss.str());
        }
        std::ifstream ifs(filepath, std::ios::binary);
        load_from_stream(ifs);
        ifs.close();
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
    
//private:
//    void read_context_and_pubkey(const std::string& pubkey_filename,
//                                 std::shared_ptr<helib::FHEcontext>& context,
//                                 std::shared_ptr<helib::FHEPubKey>& pubkey)
//    {
//        unsigned long m, p, r;
//        std::vector<long> gens, ords;
//        std::ifstream ct(pubkey_filename);
//        helib::readContextBase(ct, m, p, r, gens, ords);
//        
//        std::shared_ptr<helib::FHEcontext> context_ptr(
//            new helib::FHEcontext(m, p, r, gens, ords));
//        ct >> *context_ptr;
//        
//        std::shared_ptr<helib::FHEPubKey> pubkey_ptr(
//            new helib::FHEPubKey(*context_ptr));
//        ct >> *pubkey_ptr;
//        
//        ct.close();
//
//        context = context_ptr;
//        pubkey  = pubkey_ptr;
//    }

    
private:
    const PubKey&  pubkey_;
    size_t size_;
    std::shared_ptr<helib::Ctxt> ctxt_ptr_;
};

EncData::EncData(const PubKey& pubkey)
    : pimpl_(new Impl(pubkey))
{
}

void EncData::generate(const std::vector<long>& inputdata,
                       const Context& context)
{
    pimpl_->generate(inputdata, context);
}

//void EncData::generate(const std::vector<long>& inputdata,
//                       const std::string& pubkey_filename)
//{
//    pimpl_->generate(inputdata, pubkey_filename);
//}

void EncData::save_to_stream(std::ostream& os) const
{
    pimpl_->save_to_stream(os);
}

void EncData::load_from_stream(std::istream& is)
{
    pimpl_->load_from_stream(is);
}

void EncData::save_to_file(const std::string& filepath) const
{
    pimpl_->save_to_file(filepath);
}
    
void EncData::load_from_file(const std::string& filepath)
{
    pimpl_->load_from_file(filepath);
}
    
//void EncData::load_from_stream(std::istream& is,
//                               const std::string& pubkey_filename)
//{
//    pimpl_->load_from_stream(is, pubkey_filename);
//}
    
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
