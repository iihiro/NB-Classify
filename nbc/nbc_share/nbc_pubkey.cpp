#include <fstream>
#include <nbc_share/nbc_pubkey.hpp>

#include <helib/FHE.h>
#include <helib/EncryptedArray.h>

namespace nbc_share
{

struct PubKey::Impl
{
    Impl()
    {}

    void save_to_stream(std::ostream& os) const
    {
        helib::writePubKeyBinary(os, *pubkey_);
    }
    
    void load_from_stream(std::istream& is)
    {
        unsigned long m, p, r;
        std::vector<long> gens, ords;
        helib::readContextBase(is, m, p, r, gens, ords);
        
        context_ = std::make_shared<helib::FHEcontext>(m, p, r, gens, ords);
        is >> *context_;

        pubkey_ = std::make_shared<helib::FHEPubKey>(*context_);
        is >> *pubkey_;
    }
    
    void save_to_file(const std::string& filepath) const
    {
        std::ofstream ofs(filepath);
        save_to_stream(ofs);
        ofs.close();
    }
    
    void load_from_file(const std::string& filepath)
    {
        std::ifstream ifs(filepath);
        load_from_stream(ifs);
        ifs.close();
    }

private:
    std::shared_ptr<helib::FHEcontext> context_;
    std::shared_ptr<helib::FHEPubKey>  pubkey_;
};

PubKey::PubKey(void) : pimpl_(new Impl())
{}

void PubKey::save_to_stream(std::ostream& os) const
{
    pimpl_->save_to_stream(os);
}

void PubKey::load_from_stream(std::istream& is)
{
    pimpl_->load_from_stream(is);
}

void PubKey::save_to_file(const std::string& filepath) const
{
    pimpl_->save_to_file(filepath);
}
    
void PubKey::load_from_file(const std::string& filepath)
{
    pimpl_->load_from_file(filepath);
}
    
} /* namespace nbc_share */
