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

#include <sstream>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <nbc_share/nbc_utility.hpp>
#include <nbc_share/nbc_securekey_filemanager.hpp>

#include <helib/FHE.h>
#include <helib/EncryptedArray.h>
#include <NTL/lzz_pXFactoring.h>

namespace nbc_share
{

struct SecureKeyFileManager::Impl
{
    Impl(const std::string& pubkey_filename,
         const std::string& seckey_filename,
         const std::string& context_filename,
         const long fheM = DefaultFheM, const long fheL = DefaultFheL)
      : pubkey_filename_(pubkey_filename),
        seckey_filename_(seckey_filename),
        context_filename_(context_filename),
        m_(fheM),
        L_(fheL)
    {
    }
    ~Impl(void) = default;

    void initialize(void)
    {
        helib::FHEcontext context(m_, p_, r_);
        helib::buildModChain(context, L_, c_);
        helib::FHESecKey secretKey(context);
        const helib::FHEPubKey& publicKey = secretKey;
        secretKey.GenSecKey(w_);
        helib::addSome1DMatrices(secretKey);
        STDSC_LOG_INFO("Generated the secret key for FHE.");
        
        double security = context.securityLevel();
        long slots = context.zMStar.getNSlots();

        std::ostringstream oss;
        oss << "slots=" << slots << ", m=" << m_ << ", p=" << p_
            << ", r=" << r_ << ", L=" << L_ << ", security=" << security
            << "PhiM: " << context.zMStar.getPhiM();
        STDSC_LOG_INFO(oss.str().c_str());

        std::ofstream contextfile(context_filename_, std::ios::binary);
        helib::writeContextBaseBinary(contextfile, context);
        helib::writeContextBinary(contextfile, context);
        
        std::ofstream pubkeyfile(pubkey_filename_, std::ios::binary);
        helib::writePubKeyBinary(pubkeyfile, publicKey);
        pubkeyfile.close();

        std::ofstream seckeyfile(seckey_filename_, std::ios::binary);
        helib::writeSecKeyBinary(seckeyfile, secretKey);
        seckeyfile.close();
    }

    size_t pubkey_size(void) const
    {
        return nbc_share::utility::file_size(pubkey_filename_);
    }
    
    size_t seckey_size(void) const
    {
        return nbc_share::utility::file_size(seckey_filename_);
    }

    void pubkey_data(void* buffer)
    {
        size_t size = pubkey_size();
        std::ifstream ifs(pubkey_filename_, std::ios::binary);
        if (!ifs.is_open())
        {
            std::ostringstream oss;
            oss << "failed to open. (" << pubkey_filename_ << ")";
            STDSC_THROW_FILE(oss.str());
        }
        else
        {
            ifs.read(reinterpret_cast<char*>(buffer), size);
        }
    }

    void seckey_data(void* buffer)
    {
        size_t size = seckey_size();
        std::ifstream ifs(seckey_filename_, std::ios::binary);
        if (!ifs.is_open())
        {
            std::ostringstream oss;
            oss << "failed to open. (" << seckey_filename_ << ")";
            STDSC_THROW_FILE(oss.str());
        }
        else
        {
            ifs.read(reinterpret_cast<char*>(buffer), size);
        }
    }

    bool is_exist_pubkey(void) const
    {
        std::ifstream ifs(pubkey_filename_);
        return ifs.is_open();
    }

    bool is_exist_seckey(void) const
    {
        std::ifstream ifs(seckey_filename_);
        return ifs.is_open();
    }

    std::string pubkey_filename(void) const
    {
        return pubkey_filename_;
    }

    std::string seckey_filename(void) const
    {
        return seckey_filename_;
    }

private:
    std::string pubkey_filename_;
    std::string seckey_filename_;
    std::string context_filename_;
    long m_ = 11119;
    long L_ = 180;
    
    long p_ = 2;
    long r_ = 18;
    long c_ = 3;
    long w_ = 64;
    long d_ = 0;
};

SecureKeyFileManager::SecureKeyFileManager(const std::string& pubkey_filename,
                                           const std::string& context_filename,
                                           const std::string& seckey_filename,
                                           const long fheM, const long fheL)
  : pimpl_(new Impl(pubkey_filename,
                    seckey_filename,
                    context_filename,
                    fheM, fheL))
{
}

void SecureKeyFileManager::initialize(void)
{
    pimpl_->initialize();
}

size_t SecureKeyFileManager::pubkey_size(void) const
{
    return pimpl_->pubkey_size();
}

size_t SecureKeyFileManager::seckey_size(void) const
{
    return pimpl_->seckey_size();
}

void SecureKeyFileManager::pubkey_data(void* buffer)
{
    pimpl_->pubkey_data(buffer);
}

void SecureKeyFileManager::seckey_data(void* buffer)
{
    pimpl_->seckey_data(buffer);
}

bool SecureKeyFileManager::is_exist_pubkey(void) const
{
    return pimpl_->is_exist_pubkey();
}

bool SecureKeyFileManager::is_exist_seckey(void) const
{
    return pimpl_->is_exist_seckey();
}

std::string SecureKeyFileManager::pubkey_filename(void) const
{
    return pimpl_->pubkey_filename();
}

std::string SecureKeyFileManager::seckey_filename(void) const
{
    return pimpl_->seckey_filename();
}

} /* namespace nbc_share */
