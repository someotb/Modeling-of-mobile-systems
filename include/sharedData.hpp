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
        char c_msg[101] = "Hello World! Hello Heaven! Hello Paradise! Hello Hello!";
        std::string s_msg = "Hello World! Hello Heaven! Hello Paradise! Hello Hello!";
        std::string r_msg = "";
        std::vector<std::complex<float>> tx;

        struct hamming
        {
            std::vector<int> errs_pos;
        };
        
        hamming ham;
    };
    
    struct flags
    {
        std::atomic_bool exit{false};
        std::atomic_bool msg_r{false};
    };

    struct params
    {
        struct signal
        {
            float bandwidth = 8 * 10e6;
            float carr_freq = 2.4 * 10e9;
        };

        struct ofdm
        {
            int pilots_step = 3;
            float zero_guard = 0.2;
            float cp_len = 0.5;
        };

        struct multipath
        {
            int cnt_beam = 6;
        };

        signal s;
        ofdm o;
        multipath m;
    };
    
    data d;
    flags f;
    params p;
};
