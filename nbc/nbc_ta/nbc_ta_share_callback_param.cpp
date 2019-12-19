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
//#include <unordered_map>
//#include <stdsc/stdsc_exception.hpp>
//#include <stdsc/stdsc_buffer.hpp>
//#include <nbc_share/nbc_securekey_filemanager.hpp>
#include <nbc_ta/nbc_ta_session.hpp>
#include <nbc_ta/nbc_ta_share_callback_param.hpp>

namespace nbc_ta
{

//struct SessionContext
//{
//    int64_t result_index;
//};
//
//struct CallbackParam::Impl
//{
//    Impl(void) = default;
//    ~Impl(void) = default;
//
//    void set_skm(std::shared_ptr<nbc_share::SecureKeyFileManager>& skm)
//    {
//        skm_ = skm;
//    }
//
//    nbc_share::SecureKeyFileManager& get_skm(void)
//    {
//        STDSC_THROW_FAILURE_IF_CHECK(skm_,
//                                     "Err: SecurekeyFilemanager is not set.");
//        return *skm_;
//    }
//
//    void set_result(const int32_t session_id, const int64_t result_index)
//    {
//        sessions_[session_id].result_index = result_index;
//    }
//    
//    int64_t get_result(const int32_t session_id) const
//    {
//        int32_t result_index = -1;
//        if (sessions_.count(session_id)) {
//            result_index = sessions_.at(session_id).result_index;
//        }
//        return result_index;
//    }
//
//private:
//    std::shared_ptr<nbc_share::SecureKeyFileManager> skm_;
//    std::unordered_map<int32_t, SessionContext> sessions_;
//};

CallbackParam::CallbackParam(void)
    : sc_ptr(new SessionContainer())
{
}
//
//void CallbackParam::set_skm(
//  std::shared_ptr<nbc_share::SecureKeyFileManager>& skm)
//{
//    pimpl_->set_skm(skm);
//}
//
//nbc_share::SecureKeyFileManager& CallbackParam::get_skm(void)
//{
//    return pimpl_->get_skm();
//}
//
//void CallbackParam::set_result(const int32_t session_id, const int64_t result_index)
//{
//    pimpl_->set_result(session_id, result_index);
//}
//
//int64_t CallbackParam::get_result(const int32_t session_id) const
//{
//    return pimpl_->get_result(session_id);
//}
//
} /* namespace nbc_ta */
