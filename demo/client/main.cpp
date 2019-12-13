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
#include <string>
#include <iostream>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <nbc_client/nbc_client_ta_client.hpp>
#include <nbc_client/nbc_client_cs_client.hpp>
#include <share/define.hpp>

static constexpr const char* INFO_FILENAME   = "sample_info.csv";
static constexpr const char* TEST_FILENAME   = "sample_test.csv";

struct Option
{
    std::string info_filename;
    std::string test_filename;
};

void init(Option& option, int argc, char* argv[])
{
    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "i:t:h")) != -1)
    {
        switch (opt)
        {
            case 'i':
                option.info_filename = optarg;
                break;
            case 't':
                option.test_filename = optarg;
                break;
            case 'h':
            default:
                printf("Usage: %s \n", argv[0]);
                exit(1);
        }
    }
}

void exec(Option& option)
{
    const char* host = "localhost";

#if 0
    nbc_client::TAParam ta_param;
    ta_param.pubkey_filename = PUBKEY_FILENAME;
    nbc_client::TAClient<> ta_client(host, PORT_TA_SRV1);
    ta_client.start(ta_param);
#endif

    nbc_client::CSParam cs_param;
    cs_param.info_filename = option.info_filename;
    cs_param.test_filename = option.test_filename;
    nbc_client::CSClient<> cs_client(host, PORT_CS_SRV1);
    cs_client.start(cs_param);
    cs_client.wait();

#if 0
    int64_t result_index;
    ta_client.wait();
    ta_client.get_result(result_index);
    
    std::cout << "result: " << result_index << std::endl;
#endif
}

int main(int argc, char* argv[])
{
    STDSC_INIT_LOG();
    try
    {
        Option option;
        init(option, argc, argv);
        STDSC_LOG_INFO("Launched Client demo app");
        exec(option);
    }
    catch (stdsc::AbstractException& e)
    {
        STDSC_LOG_ERR("Err: %s", e.what());
    }
    catch (...)
    {
        STDSC_LOG_ERR("Catch unknown exception");
    }

    return 0;
}
