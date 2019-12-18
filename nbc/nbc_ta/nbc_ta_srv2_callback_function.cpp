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
#include <cstring>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_socket.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <nbc_share/nbc_packet.hpp>
#include <nbc_share/nbc_securekey_filemanager.hpp>
#include <nbc_share/nbc_encdata.hpp>
#include <nbc_share/nbc_plaindata.hpp>
#include <nbc_share/nbc_computeparam.hpp>
#include <nbc_share/nbc_context.hpp>
#include <nbc_share/nbc_pubkey.hpp>
#include <nbc_share/nbc_seckey.hpp>
#include <nbc_ta/nbc_ta_share_callback_param.hpp>
#include <nbc_ta/nbc_ta_srv2_callback_function.hpp>
#include <nbc_ta/nbc_ta_srv2_state.hpp>

#include <helib/FHE.h>
#include <helib/EncryptedArray.h>

namespace nbc_ta
{
namespace srv2
{

long compute(const nbc_share::EncData& enc_diff,
             const nbc_share::ComputeParam& cparam,
             const nbc_share::Context& context,
             const nbc_share::SecKey& seckey,
             const long plain_mod,
             const long last_index,
             std::vector<long>& vec_b)
{
    std::vector<long> vdiff;
    long b = 0;
    long new_index = last_index;;

    enc_diff.decrypt(context, seckey, vdiff);
    if (vdiff[0] < plain_mod / 2) {
        b = 1;
        new_index = cparam.index;
    } else {
        b = 0;
    }

    std::fill(vec_b.begin(), vec_b.end(), b);
    
    return new_index;
}
    
// CallbackFunctionSessionCreate
DEFUN_DOWNLOAD(CallbackFunctionSessionCreate)
{
    STDSC_LOG_INFO("Received session create. (current state : %lu)",
                   state.current_state());

    //// ここで今後の計算用にContextとPubkeyを作成して、param_に保持しておく
    //// 本来はsrv2起動時の鍵が生成されたタイミングでcallback_paramに保持したい
    //auto& skm = param_.get_skm();
    //std::shared_ptr<nbc_share::Context> context_ptr(new nbc_share::Context);
    //context_ptr->load_from_file(skm.filename(nbc_share::SecureKeyFileManager::Kind_t::kKindContext));
    //std::shared_ptr<nbc_share::PubKey>  pubkey_ptr(new nbc_share::PubKey(context_ptr->get()));
    //pubkey_ptr->load_from_file(skm.filename(nbc_share::SecureKeyFileManager::Kind_t::kKindPubKey));
    //std::shared_ptr<nbc_share::SecKey> seckey_ptr(new nbc_share::SecKey(context_ptr->get()));
    //seckey_ptr->load_from_file(skm.filename(nbc_share::SecureKeyFileManager::Kind_t::kKindSecKey));
    //
    //param_.context_ptr = context_ptr;
    //param_.pubkey_ptr  = pubkey_ptr;
    //param_.seckey_ptr  = seckey_ptr;
    
    // generate session_id
    const int32_t session_id = 456;
    const size_t size = sizeof(session_id);
    
    stdsc::Buffer buffer(size);
    std::memcpy(buffer.data(), &session_id, size);
    
    sock.send_packet(stdsc::make_data_packet(nbc_share::kControlCodeDataSessionID,
                                             sizeof(session_id)));
    sock.send_buffer(buffer);
    state.set(kEventSessionCreate);
}

// CallbackFunctionBeginComputing
DEFUN_REQUEST(CallbackFunctionBeginComputing)
{
    STDSC_LOG_INFO("Received begin computing. (current state : %lu)",
                   state.current_state());
    STDSC_THROW_CALLBACK_IF_CHECK(
        (kStateSessionCreated == state.current_state() ||
         kStateComputing == state.current_state()),
        "Warn: must be session created or computing state to receive begin computing.");

    state.set(kEventBeginRequest);
}

// CallbackFunctionComputeData
DEFUN_UPDOWNLOAD(CallbackFunctionCompute)
{
    STDSC_LOG_INFO("Received compute request. (current state : %lu)",
                   state.current_state());
    STDSC_THROW_CALLBACK_IF_CHECK(
        kStateComputable <= state.current_state(),
        "Warn: must be computable state to receive compute request.");

    const auto& context = *param_.context_ptr;
    const auto& pubkey  = *param_.pubkey_ptr;
    const auto& seckey  = *param_.seckey_ptr;
    
    stdsc::BufferStream upbuffstream(buffer);
    std::iostream upstream(&upbuffstream);

    nbc_share::EncData ust_encdata(pubkey);
    nbc_share::PlainData<nbc_share::ComputeParam> ust_pladata;
    
    ust_encdata.load_from_stream(upstream);
    ust_pladata.load_from_stream(upstream);

    const auto& ct_diff = ust_encdata.data();
    const auto& cparam  = ust_pladata.data();

    long num_slots = param_.context_ptr->get().zMStar.getNSlots();
    std::vector<long> vec_b(num_slots);

    long plain_mod  = param_.get_skm().plain_mod();
    long last_index = param_.get_result(cparam.session_id);

    nbc_share::EncData enc_diff(pubkey);
    enc_diff.push(ct_diff);
    auto new_index = compute(enc_diff, cparam, context, seckey,
                             plain_mod, last_index, vec_b);
    param_.set_result(cparam.session_id, new_index);

    nbc_share::EncData dst_encdata(pubkey);
    dst_encdata.encrypt(vec_b, context);

    auto dst_encdata_size = dst_encdata.stream_size();
    stdsc::BufferStream dlbuffstream(dst_encdata_size);
    std::iostream dlstream(&dlbuffstream);

    dst_encdata.save_to_stream(dlstream);
    stdsc::Buffer* dlbuffer = &dlbuffstream;
    sock.send_packet(
      stdsc::make_data_packet(nbc_share::kControlCodeDataComputeAck, dst_encdata_size));
    sock.send_buffer(*dlbuffer);
}
    
} /* namespace srv2 */
} /* namespace nbc_ta */