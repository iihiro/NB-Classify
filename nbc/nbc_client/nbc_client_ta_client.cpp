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

    int64_t get_result(const int32_t session_id) const
    {
        constexpr uint32_t retry_interval_usec_to_request_result = 4000000;

        auto sz = sizeof(session_id);
        stdsc::Buffer sbuffer(sz);
        std::memcpy(sbuffer.data(), static_cast<const void*>(&session_id), sz);
        
        stdsc::Buffer rbuffer;
        client_.send_recv_data_blocking(nbc_share::kControlCodeUpDownloadResult,
                                        sbuffer, rbuffer,
                                        retry_interval_usec_to_request_result);
        return *reinterpret_cast<int64_t*>(rbuffer.data());
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

int64_t TAClient::get_result(const int32_t session_id) const
{
    return pimpl_->get_result(session_id);
}

} /* namespace nbc_client */
