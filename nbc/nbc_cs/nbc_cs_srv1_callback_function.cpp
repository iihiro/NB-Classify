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
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_socket.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <nbc_share/nbc_packet.hpp>
#include <nbc_share/nbc_encdata.hpp>
#include <nbc_share/nbc_permvec.hpp>
#include <nbc_share/nbc_pubkey.hpp>
#include <nbc_share/nbc_context.hpp>
#include <nbc_share/nbc_plaindata.hpp>
#include <nbc_share/nbc_computeparam.hpp>
#include <nbc_cs/nbc_cs_share_callback_param.hpp>
#include <nbc_cs/nbc_cs_srv1_callback_function.hpp>
#include <nbc_cs/nbc_cs_client.hpp>
#include <nbc_cs/nbc_cs_srv1_state.hpp>

#include <helib/FHE.h>
#include <helib/EncryptedArray.h>

namespace nbc_cs
{
namespace srv1
{

// CallbackFunctionSessionCreate
DEFUN_DOWNLOAD(CallbackFunctionSessionCreate)
{
    STDSC_LOG_INFO("Received session create. (current state : %s)",
                   state.current_state_str().c_str());
    
#if 1
    auto& client = param_.get_client();
    auto session_id = client.create_session();
#else
    const int32_t session_id = 1234;
#endif
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
    STDSC_LOG_INFO("Received encrypted model. (current state : %s)",
                   state.current_state_str().c_str());
    
    stdsc::BufferStream buffstream(buffer);
    std::iostream stream(&buffstream);
    
    auto& client = param_.get_client();

    std::shared_ptr<nbc_share::EncData> encmodel_ptr(new nbc_share::EncData(client.pubkey()));
    encmodel_ptr->load_from_stream(stream);
    param_.encmodel_ptr = encmodel_ptr;

    std::ofstream ofs(param_.encmodel_filename);
    encmodel_ptr->save_to_stream(ofs);
    ofs.close();
    
    state.set(kEventEncModel);
}

// CallbackFunctionEncInput
#if 1
DEFUN_DATA(CallbackFunctionEncInput)
{
    STDSC_LOG_INFO("Received input data. (current state : %s)",
                   state.current_state_str().c_str());
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

    std::shared_ptr<nbc_share::PermVec> permvec_ptr(new nbc_share::PermVec());
    permvec_ptr->load_from_stream(stream);
    param_.permvec_ptr = permvec_ptr;

    encdata_ptr->save_to_file(param_.encdata_filename);

    {
        const auto& vec = permvec_ptr->vdata();
        param_.permvec.resize(vec.size(), -1);
        std::memcpy(param_.permvec.data(), vec.data(), sizeof(long) * vec.size());
    }
    
    {        
        std::ostringstream oss;
        oss << "permvec: sz=" << param_.permvec.size();
        oss << ", data=";
        for (auto& v : param_.permvec) oss << " " << v;
        STDSC_LOG_DEBUG(oss.str().c_str());
    }
    
    state.set(kEventEncInput);
    state.set(kEventPermVec);
}
#else
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
#endif

// CallbackFunctionPermVec
DEFUN_DATA(CallbackFunctionPermVec)
{
    STDSC_LOG_INFO("Received perm vector. (current state : %s)",
                   state.current_state_str().c_str());
    STDSC_THROW_CALLBACK_IF_CHECK(
        (kStateSessionCreated == state.current_state() ||
         kStateComputable     == state.current_state()),
        "Warn: must be SessionCreated or Computable state to receive encrypting input.");

    auto* p    = static_cast<const uint8_t*>(buffer.data());
    auto  num  = *reinterpret_cast<const size_t*>(p + 0);
    auto* data = static_cast<const void*>(p + sizeof(size_t));
    
    param_.permvec.resize(num, -1);
    std::memcpy(param_.permvec.data(), data, sizeof(long) * num);

    {
        std::ostringstream oss;
        oss << "permvec: sz=" << param_.permvec.size();
        oss << ", data=";
        for (auto& v : param_.permvec) oss << " " << v;
        STDSC_LOG_DEBUG(oss.str().c_str());
    }

    state.set(kEventPermVec);
}
    
// CallbackFunctionComputeRequest
DEFUN_DATA(CallbackFunctionComputeRequest)
{
    STDSC_LOG_INFO("Received compute request. (current state : %s)",
                   state.current_state_str().c_str());
    STDSC_THROW_CALLBACK_IF_CHECK(
        kStateComputable == state.current_state(),
        "Warn: must be Computable state to receive compute request.");

    auto* p          = static_cast<const uint8_t*>(buffer.data());
    auto  session_id = *reinterpret_cast<const int32_t*>(p + 0);

    STDSC_LOG_INFO("start computing of session#%d", session_id);
    
    auto& client  = param_.get_client();
    auto& context = client.context();
    
    auto  class_num   = param_.permvec.size();
    auto& ct_data     = param_.encdata_ptr->data();
    auto& model_ctxts = param_.encmodel_ptr->vdata();
    auto& permvec     = param_.permvec;

    auto& context_data = context.get();
    NTL::ZZX G = context_data.alMod.getFactorsOverZZ()[0];
    helib::EncryptedArray ea(context_data, G);

    std::vector<helib::Ctxt> res_ctxts;
    for (size_t j=0; j<class_num; ++j) {
        auto res = model_ctxts[j];
        res.multiplyBy(ct_data);
        totalSums(ea, res);
        res_ctxts.push_back(res);
    }
    STDSC_LOG_TRACE("finished calculating probability of each class");

    std::vector<helib::Ctxt> permed;
    for (size_t j=0; j<permvec.size(); ++j) {
        permed.push_back(res_ctxts[permvec[j]]);
    }
    STDSC_LOG_TRACE("Permuted the probability ciphertexts");

    client.begin_computation(session_id);

    helib::Ctxt max = permed[0];

    std::srand(std::time(nullptr));
    
    for (size_t j=1; j<class_num; ++j) {
        STDSC_LOG_INFO("computation loop (%lu / %lu)",
                       j, class_num);

        auto coeff = (std::rand() % 100) + 1;
        auto ct_diff = permed[j];
        auto tmp = max;
        ct_diff -= tmp;
        ct_diff.multByConstant(NTL::to_ZZ(coeff));
        STDSC_LOG_TRACE("computed ct_diff");

        uint32_t flag = 0;
        if (j == 1) {
            flag = COMPUTE_FLAG_BGN;
        } else if (j == class_num - 1) {
            flag = COMPUTE_FLAG_END;
        }
        nbc_share::ComputeParam cparam;
        cparam.index      = j;
        cparam.flag       = flag;
        cparam.session_id = session_id;
        STDSC_LOG_TRACE("created compute param (index:%lu, flag:0x%04x, session:%d)",
                        j, flag, session_id);
        
        auto ct_b = client.compute_on_TA(ct_diff, cparam);
        
        helib::Ctxt cur_max = max;
        max = permed[j];
        max.multiplyBy(ct_b);
        helib::Ctxt ct_temp = ct_b;
        ct_temp.addConstant(NTL::to_ZZ(-1));
        ct_temp.multiplyBy(cur_max);
        max -= ct_temp;
    }

    client.end_computation(session_id);
}

} /* namespace srv1 */
} /* namespace nbc_cs */
