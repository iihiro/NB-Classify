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
#include <vector>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_socket.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <nbc_share/nbc_packet.hpp>
#include <nbc_share/nbc_securekey_filemanager.hpp>
#include <nbc_ta/nbc_ta_share_callback_param.hpp>
#include <nbc_ta/nbc_ta_srv2_callback_function.hpp>
#include <nbc_ta/nbc_ta_srv2_state.hpp>

namespace nbc_ta
{
namespace srv2
{

// CallbackFunctionBeginComputing
DEFUN_DATA(CallbackFunctionBeginComputing)
{
    STDSC_LOG_INFO("Received begin computing. (current state : %lu)",
                   state.current_state());

    auto* p         = static_cast<const uint8_t*>(buffer.data());
    auto  class_num = *reinterpret_cast<const size_t*>(p + 0);
    
    // sessionID毎にclass_numを管理する
}

} /* namespace srv2 */
} /* namespace nbc_ta */
