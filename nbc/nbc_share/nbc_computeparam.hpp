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

#ifndef NBC_COMPUTEPARAM_HPP
#define NBC_COMPUTEPARAM_HPP

#include <iostream>
#include <nbc_share/nbc_plaindata.hpp>

namespace nbc_share
{

/**
 * @brief This clas is used to hold the parameters to compute between CS and TA.
 */
struct ComputeParam
{
    size_t index = -1;
#define COMPUTE_FLAG_BGN (0x1)
#define COMPUTE_FLAG_END (0x2)
    uint32_t flag = 0;
    int32_t session_id;
};

std::ostream& operator<<(std::ostream& os, const ComputeParam& cparam);
//{
//    os << cparam.index      << std::endl;
//    os << cparam.flag       << std::endl;
//    os << cparam.session_id << std::endl;
//    return os;
//}

std::istream& operator>>(std::istream& is, ComputeParam& cparam);
//{
//    is >> cparam.index;
//    is >> cparam.flag;
//    is >> cparam.session_id;
//    return is;
//}

} /* namespace nbc_share */

#endif /* NBC_COMPUTEPARAM_HPP */
