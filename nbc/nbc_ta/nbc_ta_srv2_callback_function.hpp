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

#ifndef NBC_TA_SRV2_CALLBACK_FUNCTION_HPP
#define NBC_TA_SRV2_CALLBACK_FUNCTION_HPP

#include <stdsc/stdsc_callback_function.hpp>

namespace nbc_ta
{
struct CallbackParam;
}

namespace nbc_ta
{
namespace srv2
{

using CallbackParam = nbc_ta::CallbackParam;

/**
 * @brief Provides callback function in receiving begin computing.
 */
DECLARE_DATA_CLASS(CallbackFunctionBeginComputing);

} /* namespace srv2 */
} /* namespace nbc_ta */

#endif /* NBC_TA_SRV2_CALLBACK_FUNCTION_HPP */
