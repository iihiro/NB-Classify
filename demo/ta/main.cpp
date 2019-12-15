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

#include <unistd.h>
#include <iostream>
#include <memory>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_callback_function_container.hpp>
#include <stdsc/stdsc_server.hpp>
#include <stdsc/stdsc_log.hpp>
#include <nbc_share/nbc_packet.hpp>
#include <nbc_share/nbc_securekey_filemanager.hpp>
#include <nbc_ta/nbc_ta_srv1.hpp>
#include <nbc_ta/nbc_ta_srv1_state.hpp>
#include <nbc_ta/nbc_ta_srv1_callback_function.hpp>
#include <nbc_ta/nbc_ta_srv2_state.hpp>
#include <nbc_ta/nbc_ta_share_callback_param.hpp>
#include <share/define.hpp>

static constexpr const char* PUBKEY_FILENAME  = "pubkey.txt";
static constexpr const char* SECKEY_FILENAME  = "seckey.txt";
static constexpr const char* CONTEXT_FILENAME = "context.txt";
static constexpr const char* ENC_RECOMMENDATION_FILENAME =
  "enc_recommendation.txt";

struct Option
{
    std::string pubkey_filename  = PUBKEY_FILENAME;
    std::string seckey_filename  = SECKEY_FILENAME;
    std::string context_filename = CONTEXT_FILENAME;
    bool is_generate_securekey   = false;
};

void init(Option& param, int argc, char* argv[])
{
    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "p:s:gh")) != -1)
    {
        switch (opt)
        {
            case 'p':
                param.pubkey_filename = optarg;
                break;
            case 's':
                param.seckey_filename = optarg;
                break;
            case 'c':
                param.context_filename = optarg;
                break;
            case 'g':
                param.is_generate_securekey = true;
                break;
            case 'h':
            default:
                printf(
                  "Usage: %s [-p pubkey_filename] [-s seckey_filename] [-c context_filename] [-g]\n",
                  argv[0]);
                exit(1);
        }
    }
}

static void exec(const Option& opt)
{
    stdsc::StateContext state1(std::make_shared<nbc_ta::srv1::StateReady>());

    stdsc::CallbackFunctionContainer callback1;
    nbc_ta::CallbackParam cb_param;
    {
        std::shared_ptr<nbc_share::SecureKeyFileManager> skm_ptr(
            new nbc_share::SecureKeyFileManager(opt.pubkey_filename,
                                                opt.seckey_filename,
                                                opt.context_filename));
        cb_param.set_skm(skm_ptr);
        
        std::shared_ptr<stdsc::CallbackFunction> cb_pubkey(
            new nbc_ta::srv1::CallbackFunctionPubkeyRequest(cb_param));
        callback1.set(nbc_share::kControlCodeDownloadPubkey, cb_pubkey);
        
        std::shared_ptr<stdsc::CallbackFunction> cb_context(
            new nbc_ta::srv1::CallbackFunctionContextRequest(cb_param));
        callback1.set(nbc_share::kControlCodeDownloadContext, cb_context);
        
        std::shared_ptr<stdsc::CallbackFunction> cb_result(
            new nbc_ta::srv1::CallbackFunctionResultRequest(cb_param));
        callback1.set(nbc_share::kControlCodeDownloadResult, cb_result);
    }

    std::shared_ptr<nbc_ta::srv1::TAServer> server1 = std::make_shared<nbc_ta::srv1::TAServer>(
        PORT_TA_SRV1, callback1, state1, cb_param.get_skm(), opt.is_generate_securekey);
    server1->start();

    std::string key;
    std::cout << "hit any key to exit server: " << std::endl;
    std::cin >> key;

    server1->stop();
    server1->wait();
}    

int main(int argc, char* argv[])
{
    STDSC_INIT_LOG();
    try
    {
        STDSC_LOG_INFO("Start TA");
        Option opt;
        init(opt, argc, argv);
        exec(opt);
    }
    catch (stdsc::AbstractException &e)
    {
        std::cerr << "catch exception: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "catch unknown exception" << std::endl;
    }
    return 0;
}
