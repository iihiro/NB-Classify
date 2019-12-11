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

#ifndef NBC_SECUREKEY_FILEMANAGER
#define NBC_SECUREKEY_FILEMANAGER

#include <memory>

namespace nbc_share
{

/**
 * @brief Manages secure key files. (public / secret key files)
 */
class SecureKeyFileManager
{
    static constexpr long DefaultFheM = 11119;
    static constexpr long DefaultFheL = 180;

public:
    enum Kind_t : int32_t
    {
        kKindUnkonw  = -1,
        kKindPubKey  = 0,
        kKindSecKey  = 1,
        kKindContext = 2,
    };
    
public:
    SecureKeyFileManager(const std::string& pubkey_filename,
                         const std::string& seckey_filename,
                         const std::string& context_filename,
                         const long fheM = DefaultFheM, const long fheL = DefaultFheL);
    ~SecureKeyFileManager(void) = default;

    void initialize(void);

    size_t size(const Kind_t kind) const;

    void data(const Kind_t kind, void* buffer);

    bool is_exist(const Kind_t kind) const;

    std::string filename(const Kind_t kind) const;

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace nbc_share */

#endif /* NBC_SECUREKEY_FILEMANAGER */