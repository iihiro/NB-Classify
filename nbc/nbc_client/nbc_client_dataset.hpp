#ifndef NBC_CLIENT_DATASET_HPP
#define NBC_CLIENT_DATASET_HPP

#include <string>
#include <vector>
#include <memory>

namespace nbc_client
{

class DataInfo;
    
class Dataset
{
public:
    Dataset(void);
    virtual ~Dataset(void) = default;

    void read(const std::string& info_filename,
              const std::string& test_filename);

    const DataInfo& info(void) const;
    const std::vector<std::vector<long>>& data(void) const;
    //std::vector<std::vector<long>>& data(void);

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

struct DataInfo
{
	int64_t class_num;
	int64_t num_features;
    std::vector<std::string> class_names;
    std::vector<std::vector<std::string>> attr_values;
};

} /* namespace nbc_client */

#endif /* NBC_CLIENT_DATASET_HPP */
