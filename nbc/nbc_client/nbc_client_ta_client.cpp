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

#include <memory>
#include <fstream>
#include <vector>
#include <cstring>
#include <stdsc/stdsc_client.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <nbc_share/nbc_packet.hpp>
#include <nbc_share/nbc_define.hpp>
#include <nbc_share/nbc_pubkey.hpp>
#include <nbc_share/nbc_context.hpp>
#include <nbc_client/nbc_client_ta_client.hpp>

namespace nbc_client
{
    
struct TAClient::Impl
{
    Impl(stdsc::Client& client)
        : client_(client)
    {}

    ~Impl(void)
    {}

    void get_result(int64_t& result)
    {
        constexpr uint32_t retry_interval_usec_to_request_result = 4000000;
        
        stdsc::Buffer result_buffer;
        client_.recv_data_blocking(nbc_share::kControlCodeDownloadResult,
                                   result_buffer,
                                   retry_interval_usec_to_request_result);
        result = *reinterpret_cast<int64_t*>(result_buffer.data());
    }

private:
    stdsc::Client& client_;
};

TAClient::TAClient(const char* host, const char* port)
    : super(host, port)
{
    auto& client = super::client();
    pimpl_ = std::make_shared<Impl>(client);
}

void TAClient::get_result(int64_t& result)
{
    pimpl_->get_result(result);
}

} /* namespace nbc_client */
