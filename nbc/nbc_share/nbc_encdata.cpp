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
        : pubkey_(pubkey)
    {
    }

    Impl(const PubKey& pubkey,
         const helib::Ctxt& ctxt)
        : pubkey_(pubkey)
    {
        //ctxt_ptr_ = std::make_shared<helib::Ctxt>(pubkey_.get());
        //*ctxt_ptr_ = ctxt;
        vctxt_.push_back(ctxt);
    }
    
    //Impl(const PubKey& pubkey,
    //     std::shared_ptr<helib::Ctxt> ctxt_ptr)
    //    : pubkey_(pubkey),
    //      size_(),
    //      ctxt_ptr_(ctxt_ptr)
    //{
    //}

    void push(const std::vector<long>& inputdata,
              const Context& context)
    {
        auto& context_data = context.get();
        auto& pubkey_data  = pubkey_.get();

        NTL::ZZX G = context_data.alMod.getFactorsOverZZ()[0];
        helib::EncryptedArray ea(context_data, G);
        
        //std::shared_ptr<helib::Ctxt> encdata_ptr(new helib::Ctxt(pubkey_data));
        //auto& ctxt = *encdata_ptr;
        helib::Ctxt ctxt(pubkey_data);
        ea.encrypt(ctxt, pubkey_data, inputdata);
        
        //ctxt_ptr_ = encdata_ptr;
        vctxt_.push_back(ctxt);
        //size_     = inputdata.size();
    }

    void push(const helib::Ctxt& ctxt)
    {
        vctxt_.push_back(ctxt);
    }

    void clear(void)
    {
        vctxt_.clear();
    }
    
    void save_to_stream(std::ostream& os) const
    {
        //if (ctxt_ptr_)
        //{
        //    os << size_ << std::endl;
        //
        //    auto& ctxt = *ctxt_ptr_;
        //    os << ctxt << std::endl;
        //}
        if (vctxt_.size() == 0) {
            return;
        }
        
        os << vctxt_.size() << std::endl;
        for (const auto& ctxt : vctxt_) {
            os << ctxt << std::endl;
        }
    }

    void load_from_stream(std::istream& is)
    {
        //is >> size_;
        //
        //auto& pubkey  = pubkey_.get();
        //std::shared_ptr<helib::Ctxt> ctxt_ptr(new helib::Ctxt(pubkey));
        //auto& ctxt = *ctxt_ptr;
        //is >> ctxt;
        //
        //ctxt_ptr_ = ctxt_ptr;

        size_t vsize;
        is >> vsize;

        clear();

        for (size_t i=0; i<vsize; ++i) {
            helib::Ctxt ctxt(pubkey_.get());
            is >> ctxt;
            vctxt_.push_back(ctxt);
        }
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
    
    //size_t size(void) const
    //{
    //    return size_;
    //}

    //const helib::Ctxt& data(void) const
    //{
    //    return *ctxt_ptr_;
    //}
    //
    //helib::Ctxt& data(void)
    //{
    //    return *ctxt_ptr_;
    //}

    const helib::Ctxt& data(void) const
    {
        STDSC_THROW_FAILURE_IF_CHECK(vctxt_.size() > 0, "Data is empty.");
        return vctxt_[0];
    }
    
    helib::Ctxt& data(void)
    {
        STDSC_THROW_FAILURE_IF_CHECK(vctxt_.size() > 0, "Data is empty.");
        return vctxt_[0];
    }

    const std::vector<helib::Ctxt>& vdata(void) const
    {
        STDSC_THROW_FAILURE_IF_CHECK(vctxt_.size() > 0, "Data is empty.");
        return vctxt_;
    }
    
    std::vector<helib::Ctxt>& vdata(void)
    {
        STDSC_THROW_FAILURE_IF_CHECK(vctxt_.size() > 0, "Data is empty.");
        return vctxt_;
    }
    
    size_t stream_size(void) const
    {
        std::ostringstream oss;
        save_to_stream(oss);
        return oss.str().size();
    }
    
private:
    const PubKey&  pubkey_;
    //size_t size_;
    //std::shared_ptr<helib::Ctxt> ctxt_ptr_;
    std::vector<helib::Ctxt> vctxt_;
};

EncData::EncData(const PubKey& pubkey)
    : pimpl_(new Impl(pubkey))
{
}

EncData::EncData(const PubKey& pubkey,
                 const helib::Ctxt& ctxt)
    : pimpl_(new Impl(pubkey))
{
}

//EncData::EncData(const PubKey& pubkey,
//                 std::shared_ptr<helib::Ctxt> ctxt_ptr)
//    : pimpl_(new Impl(pubkey, ctxt_ptr))
//{
//}

void EncData::push(const std::vector<long>& inputdata,
                   const Context& context)
{
    pimpl_->push(inputdata, context);
}

void EncData::push(const helib::Ctxt& ctxt)
{
    pimpl_->push(ctxt);
}

void EncData::clear()
{
    pimpl_->clear();
}

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
    
//size_t EncData::size(void) const
//{
//    return pimpl_->size();
//}

const helib::Ctxt& EncData::data(void) const
{
    return pimpl_->data();
}

helib::Ctxt& EncData::data(void)
{
    return pimpl_->data();
}

const std::vector<helib::Ctxt>& EncData::vdata(void) const
{
    return pimpl_->vdata();
}

std::vector<helib::Ctxt>& EncData::vdata(void)
{
    return pimpl_->vdata();
}
    
size_t EncData::stream_size(void) const
{
    return pimpl_->stream_size();
}

} /* namespace nbc_share */