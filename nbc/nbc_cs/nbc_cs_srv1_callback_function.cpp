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
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_socket.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <nbc_share/nbc_packet.hpp>
#include <nbc_share/nbc_encdata.hpp>
#include <nbc_cs/nbc_cs_share_callback_param.hpp>
#include <nbc_cs/nbc_cs_srv1_callback_function.hpp>
#include <nbc_cs/nbc_cs_srv1_state.hpp>

namespace nbc_cs
{
namespace srv1
{

// CallbackFunctionEncModel
DEFUN_DATA(CallbackFunctionEncModel)
{
    STDSC_LOG_INFO("Received encrypting model. (current state : %lu)",
                   state.current_state());
}

// CallbackFunctionEncInput
DEFUN_DATA(CallbackFunctionEncInput)
{
    STDSC_LOG_INFO("Received encrypting input. (current state : %lu)",
                   state.current_state());
    STDSC_THROW_CALLBACK_IF_CHECK(
        (kStateSessionCreated == state.current_state() ||
         kStateComputable     == state.current_state()),
        "Warn: must be SessionCreated or Computable state to receive encrypting input.");

    stdsc::BufferStream buffstream(buffer);
    std::iostream stream(&buffstream);

    std::shared_ptr<nbc_share::EncData> enc_input_ptr(new nbc_share::EncData());
    enc_input_ptr->load_from_stream(stream, param_.pubkey_filename);
    param_.enc_input_ptr = enc_input_ptr;

    std::ofstream ofs(param_.enc_input_filename);
    enc_input_ptr->save_to_stream(ofs);
    
    state.set(kEventEncInput);
    // 次回、これを呼んで enc input のファイルが生成できることを確認するところから。
}

} /* namespace srv1 */
} /* namespace nbc_cs */
