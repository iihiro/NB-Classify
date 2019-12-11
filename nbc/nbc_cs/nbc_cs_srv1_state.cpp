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

#include <mutex>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_log.hpp>
#include <nbc_cs/nbc_cs_srv2_state.hpp>

namespace nbc_cs
{
namespace srv1
{

struct StateReady::Impl
{
    Impl(void)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        STDSC_LOG_TRACE("StateReady: event#%lu", event);
        switch (static_cast<Event_t>(event))
        {
            case kEventSessionCreate:
                sc.next_state(StateSessionCreated::create());
                break;
            default:
                break;
        }
    }
};

struct StateSessionCreated::Impl
{
    Impl(const bool model_received, const bool input_received)
        : model_received_(model_received),
          input_received_(input_received)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        STDSC_LOG_TRACE("SessionCreated: event#%lu, model:%d, input:5d",
                        event, model_received_, input_received_);
        switch (static_cast<Event_t>(event))
        {
            case kEventEncModel:
                model_received_ = true;
                break;
            case kEventEncInput:
                input_received_ = true;
                break;
            default:
                break;
        }

        if (model_received_ && input_received_) {
            sc.next_state(StateComputable::create());
        }
    }

private:
    bool model_received_;
    bool input_received_;
};

struct StateComputable::Impl
{
    Impl(void)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        STDSC_LOG_TRACE("StateComputable(%lu): event#%lu", event);
        switch (static_cast<Event_t>(event))
        {
            case kEventComputeRequest:
                sc.next_state(StateComputed::create());
                break;
            case kEventSessionCreate:
                sc.next_state(StateSessionCreated::create(true, false));
                break;
            default:
                break;
        }
    }
};

// 次ここから
// Computable状態でComputeRequestを受け取ったら、
// TAとの処理ループをやって、最後にEndを送って、
// そしたら、CSはまた次の処理待ちになるわけなので、
// Computing状態なんかなくて、SessionCreateに戻る？
// それか、ComputingをComputedにリネームして、
// Computed状態でSessionCreateを受け取ったら、
// TAにSessionIDの再発行を要求するとともに、
// SessionCreated状態に戻す時に、input_receivedだけfalseにする
// SessionとEncInputは紐付けたいため、再送信してもらわないと駄目な仕様にする
//
// Computable状態でSessionCreateを受け取ったら、
// TAにSessionIDの再発行を要求するとともに、
// SessionCreated状態に戻す時に、input_receivedだけfalseにする
// SessionとEncInputは紐付けたいため、再送信してもらわないと駄目な仕様にする
struct StateComputed::Impl
{
    Impl(void)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        STDSC_LOG_TRACE("StateComputed(%lu): event#%lu", event);
        switch (static_cast<Event_t>(event))
        {
            case kEventSessionCreate:
                sc.next_state(StateSessionCreated::create(true, false));
                break;
            default:
                break;
        }
    }
};

// Ready

std::shared_ptr<stdsc::State> StateReady::create(void)
{
    return std::shared_ptr<stdsc::State>(new StateReady());
}

StateReady::StateReady(void)
  : pimpl_(new Impl())
{
}

void StateReady::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

// SessionCreated

std::shared_ptr<stdsc::State> StateSessionCreated::create(const bool model_received,
                                                          const bool input_received)
{
    return std::shared_ptr<stdsc::State>(
        new StateSessionCreated(model_received, input_received));
}

StateSessionCreated::StateSessionCreated(const bool model_received,
                                         const bool input_received)
  : pimpl_(new Impl(model_received, input_received))
{
}

void StateSessionCreated::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

// Computable

std::shared_ptr<stdsc::State> StateComputable::create()
{
    return std::shared_ptr<stdsc::State>(new StateComputable());
}

StateComputable::StateComputable(void)
    : pimpl_(new Impl())
{
}

void StateComputable::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

// Computed

std::shared_ptr<stdsc::State> StateComputed::create()
{
    return std::shared_ptr<stdsc::State>(new StateComputed());
}

StateComputed::StateComputed(void)
    : pimpl_(new Impl())
{
}

void StateComputed::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

} /* srv1 */
} /* nbc_cs */
