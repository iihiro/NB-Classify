/*
 * Copyright 2018 Yamana Laboratory, Waseda University
 * Supported by JST CREST Grant Number JPMJCR1503, Japan.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE‐2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NBC_CLIENT_DATASET_HPP
#define NBC_CLIENT_DATASET_HPP

#include <string>
#include <vector>
//#include <memory>

namespace nbc_share
{
class InfoFile;
}

namespace nbc_client
{

//class DataInfo;
    
class Dataset
{
public:
    Dataset(const nbc_share::InfoFile& info);
    virtual ~Dataset(void) = default;

    void read(const std::string& filename);

    //const DataInfo& info(void) const;
    const std::vector<std::vector<long>>& data(void) const;

private:
    //struct Impl;
    //std::shared_ptr<Impl> pimpl_;
    nbc_share::InfoFile info_;
    std::vector<std::vector<long>> data_;
};

//// 次、↓から
//// InfoはModelと共通なので、shareに持っていき、
//// Infoの読み込みは既定クラスで行うようにする
//struct DataInfo
//{
//	int64_t class_num;
//	int64_t num_features;
//    std::vector<std::string> class_names;
//    std::vector<std::vector<std::string>> attr_values;
//};

} /* namespace nbc_client */

#endif /* NBC_CLIENT_DATASET_HPP */
