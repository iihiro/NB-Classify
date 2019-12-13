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

#ifndef NBC_CLIENT_CS_CLIENT_HPP
#define NBC_CLIENT_CS_CLIENT_HPP

#include <memory>
#include <string>
#include <stdsc/stdsc_thread.hpp>

namespace nbc_client
{
class CSParam;

/**
 * @brief Provides client for Server#1 on CS.
 */
template <class T = CSParam>
class CSClient : public stdsc::Thread<T>
{
    using super = stdsc::Thread<T>;

public:
    CSClient(const char* host, const char* port);
    virtual ~CSClient(void);

    void start(T& param);
    void wait(void);

    

private:
    virtual void exec(T& args,
                      std::shared_ptr<stdsc::ThreadException> te) const;

    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

struct CSParam
{
    std::string info_filename;
    std::string test_filename;
};

} /* namespace nbc_client */

#endif /* NBC_CLIENT_CS_CLIENT_HPP */
