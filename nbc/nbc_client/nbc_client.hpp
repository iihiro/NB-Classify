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

#ifndef NBC_CLIENT_HPP
#define NBC_CLIENT_HPP

#include <memory>
//#include <string>
#include <vector>
#include <functional>
#include <nbc_share/nbc_define.hpp>

namespace nbc_client
{
    
class Dataset;
    
/**
 * @brief Provides client.
 */
class Client
{
public:
    Client(const char* ta_host, const char* ta_port,
           const char* cs_host, const char* cs_port,
           const bool dl_pubkey = true,
           const uint32_t retry_interval_usec = NBC_RETRY_INTERVAL_USEC,
           const uint32_t timeout_sec = NBC_TIMEOUT_SEC);
    virtual ~Client(void) = default;

    int32_t create_session(std::function<void(const int64_t result, void* args)> result_cb, void* args);

    void compute(const int32_t session_id,
                 const std::vector<long>& data,
                 const size_t class_num);
    
private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace nbc_client */

#endif /* NBC_CLIENT_HPP */