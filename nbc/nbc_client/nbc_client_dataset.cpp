#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#include <random>
#include <algorithm>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <nbc_share/nbc_infofile.hpp>
#include <nbc_client/nbc_client_dataset.hpp>

namespace nbc_client
{

static
std::vector<std::vector<std::string>> readData(const std::string& filename)
{
    std::vector<std::vector<std::string>> data;
    std::ifstream infile(filename);
    std::string line;

    while (std::getline(infile, line)) {
        std::vector<std::string> temp;
        std::stringstream ss(line);
        std::string value;

        while (std::getline(ss, value, ',')) {
            temp.push_back(value);
        }
        
        temp.pop_back(); //pops out the class label
        data.push_back(temp);
    }
    infile.close();
    return data;
}

#if !defined(NDEBUG)
static
void printData(const std::vector<std::vector<std::string>>& data) {
    std::cout << "Printing test data" << std::endl;
    for(size_t i=0; i<data.size(); i++) {
        std::cout << "Data " << i << std::endl;
        for (size_t j=0; j<data[i].size(); j++) {
            std::cout << data[i][j] << " ";
        }
        std::cout << std::endl;
    }
}
#endif /*if !defined(NDEBUG)*/

static
std::vector<long> parseData(const std::vector<std::string>& sample,
                            const std::vector<std::vector<std::string>>& attr_values)
{
    std::vector<long> parsed;
    for (size_t i=0; i<sample.size(); i++) {
        for (size_t j=0; j< attr_values[i].size(); j++) {
            if (sample[i] == attr_values[i][j]) {
                parsed.push_back(1);
            } else {
                parsed.push_back(0);
            }
        }
    }

    return parsed;
}

#if !defined(NDEBUG)
static    
void printParsedData(const std::vector<long>& parsed)
{
    for (size_t i=0; i<parsed.size(); i++) {
        std::cout << parsed[i] << ",";
    }
    std::cout << std::endl;
}
#endif /* if !defined(NDEBUG) */

static
std::vector<long> genPermVec(int n)
{
    std::vector<long> perm_vec(n);
    std::iota(perm_vec.begin(), perm_vec.end(), 0);

    std::random_device seed_gen;
    std::mt19937 engine(seed_gen());
    std::shuffle(perm_vec.begin(), perm_vec.end(), engine);
    
    return perm_vec;
}

std::vector<long> readPermVec(const std::string& filename)
{
    std::vector<long> perm_vec;
    std::ifstream infile(filename);
    std::string line;

    while (std::getline(infile, line)) {
        std::stringstream ss(line);
        int num;

        while (ss >> num){
            perm_vec.push_back(num);
            if (ss.peek()== ',') {
                ss.ignore();
            }
        }
    }
    return perm_vec;
}

    
Dataset::Dataset(const nbc_share::InfoFile& info)
    : info_(info)
{}

void Dataset::read(const std::string& filename)
{
    auto orig_data = readData(filename);
    STDSC_LOG_TRACE("read test data from %s.",
                    filename.c_str());
    
    data_.clear();
    for (const auto& d : orig_data) {
        std::vector<long> tmp = {1};
        auto tmp2 = parseData(d, info_.attr_values);
        tmp.insert(tmp.end(), tmp2.begin(), tmp2.end());
        data_.push_back(tmp);
    }
}
    
const std::vector<std::vector<long>>& Dataset::data(void) const
{
    return data_;
}

std::vector<long> Dataset::gen_permvec(const size_t class_num)
{
    return genPermVec(static_cast<int>(class_num));
}

std::vector<long> Dataset::read_permvec(const std::string& filename)
{
    return readPermVec(filename);
}


} /* namespace nbc_client */

