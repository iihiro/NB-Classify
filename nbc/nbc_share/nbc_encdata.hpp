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

#ifndef NBC_ENCDATA_HPP
#define NBC_ENCDATA_HPP

#include <iostream>
#include <memory>
#include <vector>

namespace helib
{
class Ctxt;
}

namespace nbc_share
{

class PubKey;
class Context;
    
/**
 * @brief This clas is used to hold the encrypted data.
 */
struct EncData
{
    EncData(const PubKey& pubkey);
    EncData(const PubKey& pubkey,
            const helib::Ctxt& ctxt);
//    EncData(const PubKey& pubkey,
//            std::shared_ptr<helib::Ctxt> ctxt_ptr);
    ~EncData(void) = default;

    void push(const std::vector<long>& inputdata,
              const Context& context);

    void push(const helib::Ctxt& ctxt);

    void clear(void);

    void save_to_stream(std::ostream& os) const;
    void load_from_stream(std::istream& is);

    void save_to_file(const std::string& filepath) const;
    void load_from_file(const std::string& filepath);
    
    size_t size(void) const;

    const helib::Ctxt& data(void) const;
    helib::Ctxt& data(void);
    
    const std::vector<helib::Ctxt>& vdata(void) const;
    std::vector<helib::Ctxt>& vdata(void);

    size_t stream_size(void) const;

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace nbc_share */

#endif /* NBC_ENCDATA_HPP */