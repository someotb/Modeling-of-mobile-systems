#pragma once

#include <string>
#include <atomic>
#include <vector>
#include <bitset>
#include <complex>

struct sharedData
{
    struct data
    {
        char c_msg[101] = "";
        std::string s_msg;
        std::vector<std::complex<float>> tx;
    };
    
    struct flags
    {
        std::atomic_bool exit{false};
        std::atomic_bool msg_r{false};
    };

    struct params
    {
        int pilots_step = 7;
        float zero_guard = 0.1;
        int cp_len = 16;
    };
    
    
    data d;
    flags f;
    params p;
};
