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

#include <fstream>
#include <cstring>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_socket.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <nbc_share/nbc_packet.hpp>
#include <nbc_share/nbc_encdata.hpp>
#include <nbc_share/nbc_pubkey.hpp>
#include <nbc_share/nbc_context.hpp>
#include <nbc_cs/nbc_cs_share_callback_param.hpp>
#include <nbc_cs/nbc_cs_srv1_callback_function.hpp>
#include <nbc_cs/nbc_cs_client.hpp>
#include <nbc_cs/nbc_cs_srv1_state.hpp>

namespace nbc_cs
{
namespace srv1
{

// CallbackFunctionSessionCreate
DEFUN_DOWNLOAD(CallbackFunctionSessionCreate)
{
    STDSC_LOG_INFO("Received session create. (current state : %lu)",
                   state.current_state());

    const int32_t session_id = 1234;
    const size_t size = sizeof(session_id);
    
    stdsc::Buffer buffer(size);
    std::memcpy(buffer.data(), &session_id, size);
    STDSC_LOG_INFO("Sending session id. (id: %d)", session_id);
    sock.send_packet(stdsc::make_data_packet(nbc_share::kControlCodeDataSessionID, size));
    sock.send_buffer(buffer);
    state.set(kEventSessionCreate);
}

// CallbackFunctionEncModel
DEFUN_DATA(CallbackFunctionEncModel)
{
    STDSC_LOG_INFO("Received encrypted model. (current state : %lu)",
                   state.current_state());
}

// CallbackFunctionEncInput
DEFUN_DATA(CallbackFunctionEncInput)
{
    STDSC_LOG_INFO("Received encrypted input. (current state : %lu)",
                   state.current_state());
    STDSC_THROW_CALLBACK_IF_CHECK(
        (kStateSessionCreated == state.current_state() ||
         kStateComputable     == state.current_state()),
        "Warn: must be SessionCreated or Computable state to receive encrypting input.");

    stdsc::BufferStream buffstream(buffer);
    std::iostream stream(&buffstream);

    auto& client = param_.get_client();

    std::shared_ptr<nbc_share::EncData> encdata_ptr(new nbc_share::EncData(client.pubkey()));
    encdata_ptr->load_from_stream(stream);
    param_.encdata_ptr = encdata_ptr;

    std::ofstream ofs(param_.encdata_filename);
    encdata_ptr->save_to_stream(ofs);
    ofs.close();
    
    state.set(kEventEncInput);
}

// CallbackFunctionComputeRequest
DEFUN_REQUEST(CallbackFunctionComputeRequest)
{
    STDSC_LOG_INFO("Received compute request. (current state : %lu)",
                   state.current_state());
    STDSC_THROW_CALLBACK_IF_CHECK(
        kStateComputable == state.current_state(),
        "Warn: must be Computable state to receive compute request.");
    
}

} /* namespace srv1 */
} /* namespace nbc_cs */
