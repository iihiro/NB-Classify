#include <iostream>
#include <fstream>
#include <sstream>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <nbc_share/nbc_infofile.hpp>
#include <nbc_client/nbc_client_dataset.hpp>

namespace nbc_client
{

//DataInfo readInfo(const std::string& filename)
//{
//    DataInfo info;
//
//    std::ifstream infile(filename);
//    std::string line;
//    int count = 0;
//    info.num_features = 0;
//
//    if (!infile.is_open()) {
//        std::ostringstream oss;
//        oss << "Failed to open file. (" << filename << ")";
//        STDSC_THROW_FILE(oss.str());
//    }
//
//    while (std::getline(infile, line)) {
//        std::stringstream ss(line);
//
//        if (count == 0) {
//            count++;
//            while (ss.good()) {
//                std::string substr;
//                std::getline(ss, substr, ',');
//                info.class_names.push_back(substr);
//            }
//        } else {
//            std::vector<std::string> temp;
//            while (ss.good()) {
//                std::string substr;
//                std::getline(ss, substr, ',');
//                temp.push_back(substr);
//            }
//            info.num_features += temp.size();
//            info.attr_values.push_back(temp);
//        }
//    }
//    info.class_num = info.class_names.size();
//
//    return info;
//}

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

void printParsedData(const std::vector<long>& parsed)
{
    for (size_t i=0; i<parsed.size(); i++) {
        std::cout << parsed[i] << ",";
    }
    std::cout << std::endl;
}
    
//struct Dataset::Impl
//{
//    Impl(const nbc_share::InfoFile& info)
//        : info_(info)
//    {
//    }
//
//    ~Impl(void)
//    {}
//
//    void read(const std::string& filename)
//    {
//        auto orig_data = readData(filename);
//        STDSC_LOG_TRACE("read test data from %s.",
//                        filename.c_str());
//
//        data_.clear();
//        for (const auto& d : orig_data) {
//            std::vector<long> tmp = {1};
//            auto tmp2 = parseData(d, info_.attr_values);
//            tmp.insert(tmp.end(), tmp2.begin(), tmp2.end());
//            //tmp.resize(num_slots); //-> encrypt時にリサイズするようにする
//            data_.push_back(tmp);
//        }
//    }
//
////    const DataInfo& info(void) const
////    {
////        return info_;
////    }
//    
//    const std::vector<std::vector<long>>& data(void) const
//    {
//        return data_;
//    }
//
//private:
//    //nbc_share::InfoFile info_;
//    std::vector<std::vector<long>> data_;
//};

Dataset::Dataset(const nbc_share::InfoFile& info)
    : info_(info)
//    : pimpl_(new Impl(info))
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
        //tmp.resize(num_slots); //-> encrypt時にリサイズするようにする
        data_.push_back(tmp);
    }
    //return pimpl_->read(filename);
}
    
//const DataInfo& Dataset::info(void) const
//{
//    return pimpl_->info();
//}
    
const std::vector<std::vector<long>>& Dataset::data(void) const
{
    return data_;
    //return pimpl_->data();
}


} /* namespace nbc_client */

