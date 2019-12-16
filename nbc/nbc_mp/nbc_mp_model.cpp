#include <iostream>
#include <fstream>
#include <sstream>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <nbc_share/nbc_infofile.hpp>
#include <nbc_mp/nbc_mp_model.hpp>

namespace nbc_mp
{

//std::vector<std::vector<std::string>> readData(const std::string& filename)
//{
//    std::vector<std::vector<std::string>> data;
//    std::ifstream infile(filename);
//    std::string line;
//
//    while (std::getline(infile, line)) {
//        std::vector<std::string> temp;
//        std::stringstream ss(line);
//        std::string value;
//
//        while (std::getline(ss, value, ',')) {
//            temp.push_back(value);
//        }
//        
//        temp.pop_back(); //pops out the class label
//        data.push_back(temp);
//    }
//    infile.close();
//    return data;
//}
//
//void printData(const std::vector<std::vector<std::string>>& data) {
//    std::cout << "Printing test data" << std::endl;
//    for(size_t i=0; i<data.size(); i++) {
//        std::cout << "Data " << i << std::endl;
//        for (size_t j=0; j<data[i].size(); j++) {
//            std::cout << data[i][j] << " ";
//        }
//        std::cout << std::endl;
//    }
//}
//
//std::vector<long> parseData(const std::vector<std::string>& sample,
//                            const std::vector<std::vector<std::string>>& attr_values)
//{
//    std::vector<long> parsed;
//    for (size_t i=0; i<sample.size(); i++) {
//        for (size_t j=0; j< attr_values[i].size(); j++) {
//            if (sample[i] == attr_values[i][j]) {
//                parsed.push_back(1);
//            } else {
//                parsed.push_back(0);
//            }
//        }
//    }
//
//    return parsed;
//}
//
//void printParsedData(const std::vector<long>& parsed)
//{
//    for (size_t i=0; i<parsed.size(); i++) {
//        std::cout << parsed[i] << ",";
//    }
//    std::cout << std::endl;
//}
    
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
//            data_.push_back(tmp);
//        }
//    }
//    
//    const std::vector<std::vector<long>>& data(void) const
//    {
//        return data_;
//    }
//
//private:
//    nbc_share::InfoFile info_;
//    std::vector<std::vector<long>> data_;
//};

Model::Model(const nbc_share::InfoFile& info)
    : info_(info)
//    : pimpl_(new Impl(info))
{}

void Model::read(const std::string& filename)
{
    //Model readModel (string filename, vector <string> class_names, vector <vector <string> > attr_values){
//  Model NB;
    
    class_num_ = info_.class_names.size(); //Number of classes
    feature_num_ = info_.attr_values.size(); //Number of features
    //cout << "Number of features: " << feature_num_ << endl;
    //feature_value_num_; //Number of possible values per feature
    for (int i = 0; i<feature_num_; i++) {
        feature_value_num_.push_back(info_.attr_values[i].size());
    } // array that contains number of possible values for each attribute
    
    std::ifstream infile(filename);
    std::string line;

    int num;
    int count = 0;
    int class_count = -1;
    
    while (getline(infile, line)) {
        std::stringstream ss(line);
        
        //Reads class probabilities into class_prob
        if (count == 0) {
            while (ss >> num) {
                std::vector<long> temp;
                temp.push_back(num);
                probs_.push_back(temp);
                if (ss.peek()== ',') {
                    ss.ignore();
                }
            }
        }
        else if (((feature_num_*class_count)+2+class_count <= count) &&
                 (count <= ((feature_num_*class_count)+1+class_count+feature_num_))) {
            while (ss >> num) {
                probs_[class_count].push_back(num);
                if (ss.peek()== ',') {
                    ss.ignore();
                }
            }
        } else {
            class_count += 1;
        }
        
        count += 1;
    }
    
    //return pimpl_->read(filename);
}

void Model::print(void) const
{
    int class_num = probs_.size();
    
    std::cout << "Class probability" << std::endl;
    for (int i=0; i<class_num; i++) {
        std::cout << "Class " + std::to_string(i) + ": " << probs_[i][0] << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Conditional probability" << std::endl;
    for (int i = 0; i<class_num; i++) {
        std::cout << std::endl;
        std::cout << "Class " + std::to_string(i) << std::endl;
        //int count = 1;
        for (size_t j = 0; j<probs_[i].size(); j++) {
            std::cout << probs_[i][j] << " ";
        }
        std::cout << std::endl;
    }
}
    
} /* namespace nbc_mp */

