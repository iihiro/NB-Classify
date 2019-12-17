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
    {}
    
    const PubKey&  pubkey_;
};

EncData::EncData(const PubKey& pubkey)
    : pimpl_(new Impl(pubkey))
{}

void EncData::push(const std::vector<long>& inputdata,
                   const Context& context)
{
    auto& context_data = context.get();
    auto& pubkey_data  = pimpl_->pubkey_.get();

    NTL::ZZX G = context_data.alMod.getFactorsOverZZ()[0];
    helib::EncryptedArray ea(context_data, G);
        
    helib::Ctxt ctxt(pubkey_data);
    ea.encrypt(ctxt, pubkey_data, inputdata);
        
    vec_.push_back(ctxt);
}

void EncData::save_to_stream(std::ostream& os) const
{
    if (vec_.size() == 0) {
        return;
    }
    
    os << vec_.size() << std::endl;
    for (const auto& v : vec_) {
        os << v << std::endl;
    }
}

void EncData::load_from_stream(std::istream& is)
{
    size_t sz;
    is >> sz;

    clear();

    for (size_t i=0; i<sz; ++i) {
        helib::Ctxt ctxt(pimpl_->pubkey_.get());
        is >> ctxt;
        vec_.push_back(ctxt);
    }
}

} /* namespace nbc_share */
