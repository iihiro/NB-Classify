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
#include <cassert>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <nbc_share/nbc_pubkey.hpp>
#include <nbc_share/nbc_infofile.hpp>
#include <nbc_client/nbc_client.hpp>
#include <nbc_client/nbc_client_dataset.hpp>
#include <share/define.hpp>

#define USE_TEST_PERMVEC

static constexpr const char* INFO_FILENAME   = "../../../datasets/sample_info.csv";
static constexpr const char* TEST_FILENAME   = "../../../datasets/sample_test.csv";
static constexpr const char* PVEC_FILENAME   = "../../../testdata/permvec.txt"; // for test mode
static constexpr const char* PUBKEY_FILENAME = "pubkey.txt";

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

struct ResultParam
{
    std::vector<int64_t> indexes;
};

void result_cb(const std::vector<int64_t>& indexes, void* args)
{
    //std::cout << "called result callback. result_index:" << result << std::endl;
    std::cout << "called result callback. " << std::endl;
    auto* param = static_cast<ResultParam*>(args);
    for (const auto& index : indexes) {
        param->indexes.push_back(index);
    }
}

void exec(Option& option)
{
    const char* host   = "localhost";
    int32_t session_id = -1;
    ResultParam args;

    nbc_client::Client client(host, PORT_TA_SRV1,
                              host, PORT_CS_SRV1);

    nbc_share::InfoFile info;
    info.read(INFO_FILENAME);
    
    nbc_client::Dataset dataset(info);
    dataset.read(TEST_FILENAME);
    
    std::vector<nbc_share::PermVec> permvecs;

    size_t i = 0;
    for (const auto& data : dataset.data()) {

        std::cout << "Classifying data " << i++ << std::endl;
        
        session_id = client.create_session(result_cb, &args);
        std::cout << "session_id: " << session_id << std::endl;
        
        nbc_share::PermVec permvec;
#if defined(ENABLE_TEST_MODE)
        permvec.load_from_csvfile(PVEC_FILENAME);
#else
        permvec.gen_permvec(info.class_num);
#endif
        permvecs.push_back(permvec);
        
        client.compute(session_id, data, permvec,
                       info.class_num, info.num_features);
    }
    
    client.wait();

    assert(args.indexes.size() == dataset.data().size());
    for (size_t i=0; i<args.indexes.size(); ++i) {
        auto& permvec = permvecs[i];
        auto& index   = args.indexes[i];
        auto depermed = (permvec.vdata())[index];

        printf("Classification result of %ld: index=%ld, class=%s\n",
               i+1, depermed, info.class_names[depermed].c_str());
    }
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
