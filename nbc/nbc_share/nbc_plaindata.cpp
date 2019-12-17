#include <vector>
#include <stdsc/stdsc_exception.hpp>
#include <nbc_share/nbc_utility.hpp>
#include <nbc_share/nbc_plaindata.hpp>

namespace nbc_share
{

template <class T>
void PlainData<T>::save_to_stream(std::ostream& os) const
{
    if (super::vec_.size() == 0) {
        return;
    }
    
    os << super::vec_.size() << std::endl;
    for (const auto& v : super::vec_) {
        os << v << std::endl;
    }
}

template <class T>
void PlainData<T>::load_from_stream(std::istream& is)
{
    size_t sz;
    is >> sz;
    
    super::clear();
    
    for (size_t i=0; i<sz; ++i) {
        T v;
        is >> v;
        super::vec_.push_back(v);
    }
}

template class PlainData<long>;
    
} /* namespace nbc_share */
