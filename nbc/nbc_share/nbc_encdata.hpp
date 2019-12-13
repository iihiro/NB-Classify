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

/**
 * @brief This clas is used to hold the encrypted data.
 */
struct EncData
{
    EncData(void);
    ~EncData(void) = default;

    void generate(const std::vector<long>& inputdata,
                  const std::string& pubkey_filename);

    void save_to_stream(std::ostream& os) const;

    void load_from_stream(std::istream& is, const std::string& pubkey_filename);

    size_t size(void) const;

    const helib::Ctxt& data(void) const;
    helib::Ctxt& data(void);

    size_t stream_size(void) const;

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace nbc_share */

#endif /* NBC_ENCDATA_HPP */
