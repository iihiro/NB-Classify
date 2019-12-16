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

#ifndef NBC_ENCMODEL_HPP
#define NBC_ENCMODEL_HPP

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
 * @brief This clas is used to hold the encrypted model.
 */
struct EncModel
{
    EncModel(const PubKey& pubkey);
    ~EncModel(void) = default;

    void generate(const std::vector<long>& inputdata, // ここをモデル向けに修正
                  const Context& context);

    void save_to_stream(std::ostream& os) const;
    void load_from_stream(std::istream& is);

    void save_to_file(const std::string& filepath) const;
    void load_from_file(const std::string& filepath);
    
    size_t size(void) const;

    const std::vector<helib::Ctxt>& data(void) const;
    std::vector<helib::Ctxt>& data(void);

    size_t stream_size(void) const;

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace nbc_share */

#endif /* NBC_ENCMODEL_HPP */
