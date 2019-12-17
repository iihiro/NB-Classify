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
#include <nbc_share/nbc_plaindata.hpp>
#include <nbc_share/nbc_computeparam.hpp>
#include <nbc_cs/nbc_cs_ta_client.hpp>
#include <nbc_cs/nbc_cs_client.hpp>
#include <helib/FHE.h>
#include <helib/EncryptedArray.h>

namespace nbc_cs
{
    
struct Client::Impl
{
    Impl(const char* ta_host, const char* ta_port,
         const bool dl_pubkey,
         const uint32_t retry_interval_usec,
         const uint32_t timeout_sec)
        : ta_client_(new TAClient(ta_host, ta_port)),
          retry_interval_usec_(retry_interval_usec),
          timeout_sec_(timeout_sec)
    {
        STDSC_IF_CHECK(dl_pubkey, "False of dl_pubkey is not supported yet.");

        ta_client_->connect(retry_interval_usec_, timeout_sec_);

        context_ = std::make_shared<nbc_share::Context>();
        ta_client_->get_context(*context_);
        
        pubkey_ = std::make_shared<nbc_share::PubKey>(context_->get());
        ta_client_->get_pubkey(*pubkey_);
    }

    ~Impl(void)
    {
        ta_client_->disconnect();
    }

    const nbc_share::PubKey& pubkey(void) const
    {
        return *pubkey_;
    }

    const nbc_share::Context& context(void) const
    {
        return *context_;
    }

    helib::Ctxt compute_on_TA(const helib::Ctxt& ct_diff,
                              const nbc_share::ComputeParam& cparam)
    {
        nbc_share::EncData encdata(pubkey());
        encdata.push(ct_diff);
        nbc_share::PlainData<nbc_share::ComputeParam> plaindata;
        plaindata.push(cparam);

        auto sz = encdata.stream_size() + plaindata.stream_size();
        stdsc::BufferStream bufferstream(sz);
        std::iostream stream(&bufferstream);

        encdata.save_to_stream(stream);
        plaindata.save_to_stream(stream);

        stdsc::Buffer* sbuffer = &bufferstream;
        stdsc::Buffer rbuffer;

        ta_client_->compute(*sbuffer, rbuffer);

        nbc_share::EncData encresult(pubkey());
        {
            stdsc::BufferStream bufferstream(rbuffer);
            std::iostream stream(&bufferstream);
            encresult.load_from_stream(stream);
        }

        return encresult.data();
    }

private:
    std::shared_ptr<TAClient> ta_client_;
    const uint32_t retry_interval_usec_;
    const uint32_t timeout_sec_;
    std::shared_ptr<nbc_share::Context> context_;
    std::shared_ptr<nbc_share::PubKey> pubkey_;
};

Client::Client(const char* ta_host, const char* ta_port,
               const bool dl_pubkey,
               const uint32_t retry_interval_usec,
               const uint32_t timeout_sec)
    : pimpl_(new Impl(ta_host, ta_port,
                      dl_pubkey, retry_interval_usec, timeout_sec))
{
}

const nbc_share::PubKey& Client::pubkey(void) const
{
    return pimpl_->pubkey();
}

const nbc_share::Context& Client::context(void) const
{
    return pimpl_->context();
}

helib::Ctxt Client::compute_on_TA(const helib::Ctxt& ct_diff,
                                  const nbc_share::ComputeParam& cparam)
{
    return pimpl_->compute_on_TA(ct_diff, cparam);
}
    
} /* namespace nbc_cs */
