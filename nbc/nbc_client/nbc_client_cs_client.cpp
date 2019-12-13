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
#include <nbc_client/nbc_client_cs_client.hpp>

namespace nbc_client
{
    
template <class T>
struct CSClient<T>::Impl
{
    std::shared_ptr<stdsc::ThreadException> te_;

    Impl(const char* host, const char* port)
        : host_(host),
          port_(port)
    {
        te_ = stdsc::ThreadException::create();
    }

    int32_t create_session(void)
    {
        stdsc::Buffer session_id;
        client_.recv_data_blocking(nbc_share::kControlCodeDownloadSessionID,
                                   session_id);

        return *reinterpret_cast<int32_t*>(session_id.data());
    }

    void exec(T& args, std::shared_ptr<stdsc::ThreadException> te)
    {
        try
        {
            constexpr uint32_t retry_interval_usec = NBC_RETRY_INTERVAL_USEC;
            constexpr uint32_t timeout_sec = NBC_TIMEOUT_SEC;

            STDSC_LOG_INFO("Connecting to Server#1 on CS.");
            client_.connect(host_, port_, retry_interval_usec, timeout_sec);
            STDSC_LOG_INFO("Connected to Server#1 on CS.");
            
#if 0
            auto sid = create_session();
            STDSC_LOG_INFO("Session ID: %d", sid);
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
    stdsc::Client client_;
};

template <class T>
CSClient<T>::CSClient(const char* host, const char* port)
  : pimpl_(new Impl(host, port))
{
}

template <class T>
CSClient<T>::~CSClient(void)
{
    super::join();
}

template <class T>
void CSClient<T>::start(T& param)
{
    super::start(param, pimpl_->te_);
}

template <class T>
void CSClient<T>::wait(void)
{
    super::join();
    pimpl_->te_->rethrow_if_has_exception();
}

template <class T>
void CSClient<T>::exec(T& args,
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

template class CSClient<CSParam>;

} /* namespace nbc_client */
