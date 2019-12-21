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
#include <unordered_map>
#include <mutex>
#include <stdsc/stdsc_exception.hpp>
#include <nbc_ta/nbc_ta_session.hpp>

namespace nbc_ta
{

Session::Session(void)
{
    initialize();
}

void Session::initialize(void)
{
    result_indexes.resize(1, 0);
    is_computed  = false;
}

const std::vector<int64_t>& Session::get_results(void) const
{
    return result_indexes;
}
    
bool Session::computed(void) const
{
    return is_computed;
}


struct SessionContainer::Impl
{
    Impl(void) = default;
    ~Impl(void) = default;

    void initialize(const int32_t session_id)
    {
        auto& s = map_[session_id];
        s.initialize();
    }
    
    void set_result(const int32_t session_id,
                    const int64_t result_index)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& s = map_[session_id];
        if (s.result_indexes.size() > 0) {
            s.result_indexes[0] = result_index;
        } else {
            s.result_indexes.push_back(result_index);
        }
    }

    void push_results(const int32_t session_id,
                      const std::vector<int64_t>& result_indexes)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& s = map_[session_id];
        //std::copy(result_indexes.begin(), result_indexes.end(), s.result_indexes.end());
        for (const auto& index : result_indexes) {
            s.result_indexes.push_back(index);
        }
    }

    void set_computed(const int32_t session_id,
                      const bool is_computed)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& s = map_[session_id];
        s.is_computed = is_computed;
    }

    const bool has_context(const int32_t session_id) const
    {
        return map_.count(session_id) > 0;
    }
    
    const Session& get(const int32_t session_id) const
    {
        return map_.at(session_id);
    }

private:
    std::mutex mutex_;
    std::unordered_map<int32_t, Session> map_;
};

SessionContainer::SessionContainer(void)
    : pimpl_(new Impl())
{
}

void SessionContainer::initialize(const int32_t session_id)
{
    pimpl_->initialize(session_id);
}

void SessionContainer::set_result(const int32_t session_id,
                                  const int64_t result_index)
{
    pimpl_->set_result(session_id, result_index);
}

void SessionContainer::push_results(const int32_t session_id,
                                    const std::vector<int64_t>& result_indexes)
{
    pimpl_->push_results(session_id, result_indexes);
}

void SessionContainer::set_computed(const int32_t session_id,
                                    const bool is_computed)
{
    pimpl_->set_computed(session_id, is_computed);
}

const bool SessionContainer::has_context(const int32_t session_id) const
{
    return pimpl_->has_context(session_id);
}

const Session& SessionContainer::get(const int32_t session_id) const
{
    return pimpl_->get(session_id);
}

} /* namespace nbc_ta */
