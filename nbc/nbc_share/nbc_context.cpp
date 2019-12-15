#include <fstream>
#include <stdsc/stdsc_exception.hpp>
#include <nbc_share/nbc_context.hpp>
#include <nbc_share/nbc_utility.hpp>

#include <helib/FHE.h>
#include <helib/EncryptedArray.h>

namespace nbc_share
{

//helib::FHEcontext readContext(std::ifstream& ifs) {
//    unsigned long m1, p1, r1;
//    std::vector<long> gens, ords;
//    helib::readContextBaseBinary(ifs, m1, p1, r1, gens, ords);
//    helib::FHEcontext context(m1, p1, r1, gens, ords);
//    helib::readContextBinary(ifs, context);
//    ifs.close();
//    return context;
//}

    
struct Context::Impl
{
    Impl()
    {}

    void save_to_stream(std::ostream& os) const
    {
    }
    
    void load_from_stream(std::istream& is)
    {
        unsigned long m1, p1, r1;
        std::vector<long> gens, ords;
        helib::readContextBaseBinary(is, m1, p1, r1, gens, ords);
        
        data_ = std::make_shared<helib::FHEcontext>(m1, p1, r1, gens, ords);
        helib::readContextBinary(is, *data_);
    }
    
    void save_to_file(const std::string& filepath) const
    {
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

    const helib::FHEcontext& get(void) const
    {
        return *data_;
    }
    
private:
    std::shared_ptr<helib::FHEcontext> data_;
};

Context::Context(void) : pimpl_(new Impl())
{}

void Context::save_to_stream(std::ostream& os) const
{
    pimpl_->save_to_stream(os);
}

void Context::load_from_stream(std::istream& is)
{
    pimpl_->load_from_stream(is);
}

void Context::save_to_file(const std::string& filepath) const
{
    pimpl_->save_to_file(filepath);
}
    
void Context::load_from_file(const std::string& filepath)
{
    pimpl_->load_from_file(filepath);
}

const helib::FHEcontext& Context::get(void) const
{
    return pimpl_->get();
}
    
} /* namespace nbc_share */
