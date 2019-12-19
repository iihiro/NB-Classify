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
#include <nbc_share/nbc_encdata.hpp>
#include <nbc_share/nbc_infofile.hpp>
#include <nbc_share/nbc_permvec.hpp>
#include <nbc_client/nbc_client_ta_client.hpp>
#include <nbc_client/nbc_client_cs_client.hpp>
#include <nbc_client/nbc_client_dataset.hpp>
#include <nbc_client/nbc_client_result_thread.hpp>
#include <nbc_client/nbc_client.hpp>

#include <helib/FHE.h>
#include <helib/EncryptedArray.h>

namespace nbc_client
{
    
struct Client::Impl
{
    struct ResultCallback
    {
        std::shared_ptr<ResultThread<>> thread;
        ResultThreadParam param;
    };
    
    Impl(const char* ta_host, const char* ta_port,
         const char* cs_host, const char* cs_port,
         const bool dl_pubkey,
         const uint32_t retry_interval_usec,
         const uint32_t timeout_sec)
        : ta_client_(new TAClient(ta_host, ta_port)),
          cs_client_(new CSClient(cs_host, cs_port)),
          retry_interval_usec_(retry_interval_usec),
          timeout_sec_(timeout_sec)
    {
        STDSC_IF_CHECK(dl_pubkey, "False of dl_pubkey is not supported yet.");

        ta_client_->connect(retry_interval_usec_, timeout_sec_);
        cs_client_->connect(retry_interval_usec_, timeout_sec_);

        context_ = std::make_shared<nbc_share::Context>();
        ta_client_->get_context(*context_);
        
        pubkey_ = std::make_shared<nbc_share::PubKey>(context_->get());
        ta_client_->get_pubkey(*pubkey_);
    }

    ~Impl(void)
    {
        ta_client_->disconnect();
        cs_client_->disconnect();
    }

    int32_t create_session(std::function<void(const int64_t result, void* args)> result_cbfunc,
                           void* result_cbargs)
    {
        auto session_id = cs_client_->send_session_create();

        ResultCallback rcb;
        rcb.thread = std::make_shared<ResultThread<>>(*ta_client_, result_cbfunc, result_cbargs);
        rcb.param  = {session_id};
        cbmap_[session_id] = rcb;
        cbmap_[session_id].thread->start(cbmap_[session_id].param);
        
        return session_id;
    }
    
    void compute(const int32_t session_id,
                 const std::vector<long>& data,
                 const nbc_share::PermVec& permvec,
                 const size_t class_num)
    {
        auto& context = *context_;
        auto& pubkey  = *pubkey_;
        
        auto& context_data = context.get();
        const auto num_slots = context_data.zMStar.getNSlots();
        std::vector<long> inputdata(num_slots);
        std::copy(data.begin(), data.end(), inputdata.begin());
        
        STDSC_LOG_DEBUG("data.size=%lu, inputdata.size=%lu", data.size(), inputdata.size());

        nbc_share::EncData encdata(pubkey);
        encdata.encrypt(inputdata, context);
        encdata.save_to_file("encdata.txt");

        cs_client_->send_input(session_id, encdata, permvec);
        cs_client_->send_compute_request(session_id);
    }

    void wait(const int32_t session_id)
    {
        STDSC_LOG_TRACE("waiting result for session#%d",
                        session_id);
        if (session_id == Client::ALL_SESSION) {
            for (auto& pair : cbmap_) {
                auto& rcb = pair.second;
                rcb.thread->wait();
            }
        } else if (cbmap_.count(session_id)) {
            auto& rcb  = cbmap_.at(session_id);
            rcb.thread->wait();
        }
    }

private:
    std::shared_ptr<TAClient> ta_client_;
    std::shared_ptr<CSClient> cs_client_;
    const uint32_t retry_interval_usec_;
    const uint32_t timeout_sec_;
    std::shared_ptr<nbc_share::Context> context_;
    std::shared_ptr<nbc_share::PubKey> pubkey_;
    std::unordered_map<int32_t, ResultCallback> cbmap_;
};

Client::Client(const char* ta_host, const char* ta_port,
               const char* cs_host, const char* cs_port,
               const bool dl_pubkey,
               const uint32_t retry_interval_usec,
               const uint32_t timeout_sec)
    : pimpl_(new Impl(ta_host, ta_port, cs_host, cs_port,
                      dl_pubkey, retry_interval_usec, timeout_sec))
{
}

int32_t Client::create_session(std::function<void(const int64_t result, void* args)> result_cbfunc,
                               void* result_cbargs)
{
    return pimpl_->create_session(result_cbfunc, result_cbargs);
}
    
void Client::compute(const int32_t session_id,
                     const std::vector<long>& data,
                     const nbc_share::PermVec& permvec,
                     const size_t class_num)
{
    pimpl_->compute(session_id, data, permvec, class_num);
}

void Client::wait(const int32_t session_id)
{
    pimpl_->wait(session_id);
}
    
} /* namespace nbc_client */
