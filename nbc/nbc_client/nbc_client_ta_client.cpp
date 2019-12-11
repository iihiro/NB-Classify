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
#include <nbc_client/nbc_client_ta_client.hpp>

namespace nbc_client
{
    
template <class T>
struct TAClient<T>::Impl
{
    std::shared_ptr<stdsc::ThreadException> te_;

    Impl(const char* host, const char* port)
        : host_(host),
          port_(port),
          result_(-1)
    {
        te_ = stdsc::ThreadException::create();
    }

    void get_result(int64_t& result)
    {
        result = result_;
    }

    void download_pubkey(const std::string& out_filename)
    {
        stdsc::Buffer pubkey;
        client_.recv_data_blocking(nbc_share::kControlCodeDownloadPubkey,
                                   pubkey);

        auto data = reinterpret_cast<const char*>(pubkey.data());
        auto size = pubkey.size();
        STDSC_LOG_INFO("Saved a public key file. (%s)", out_filename.c_str());

        std::ofstream ofs(out_filename, std::ios::binary);
        ofs.write(data, size);
    }

    void exec(T& args, std::shared_ptr<stdsc::ThreadException> te)
    {
        try
        {
            constexpr uint32_t retry_interval_usec = NBC_RETRY_INTERVAL_USEC;
            //constexpr uint32_t retry_interval_usec_to_request_result = 4000000;
            constexpr uint32_t timeout_sec = NBC_TIMEOUT_SEC;

            STDSC_LOG_INFO("Connecting to Server#1 on TA.");
            client_.connect(host_, port_, retry_interval_usec, timeout_sec);
            STDSC_LOG_INFO("Connected to Server#1 on TA.");

            download_pubkey(args.pubkey_filename);
#if 0
            stdsc::Buffer result_buffer;
            client_.recv_data_blocking(nbc_share::kControlCodeDownloadResult,
                                       result_buffer,
                                       retry_interval_usec_to_request_result);
            result_ = *reinterpret_cast<int64_t*>(result_buffer.data());
#endif       
            client_.close();
        }
        catch (const stdsc::AbstractException& e)
        {
            STDSC_LOG_TRACE("Failed to client process (%s)", e.what());
            te->set_current_exception();
        }
    }

private:
    const char* host_;
    const char* port_;
    int64_t result_;
    stdsc::Client client_;
};

template <class T>
TAClient<T>::TAClient(const char* host, const char* port)
  : pimpl_(new Impl(host, port))
{
}

template <class T>
TAClient<T>::~TAClient(void)
{
    super::join();
}

template <class T>
void TAClient<T>::start(T& param)
{
    super::start(param, pimpl_->te_);
}

template <class T>
void TAClient<T>::wait(void)
{
    super::join();
    pimpl_->te_->rethrow_if_has_exception();
}

template <class T>
void TAClient<T>::get_result(int64_t& result)
{
    this->wait();
    pimpl_->get_result(result);
}

template <class T>
void TAClient<T>::exec(T& args,
                        std::shared_ptr<stdsc::ThreadException> te) const
{
    try
    {
        pimpl_->exec(args, te);
    }
    catch (...)
    {
        te->set_current_exception();
    }
}

template class TAClient<TAParam>;

} /* namespace nbc_client */
