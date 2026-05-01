#pragma once

#include <string>
#include <atomic>
#include <vector>
#include <bitset>

struct sharedData
{
    struct data
    {
        char c_msg[101] = "";
        std::string s_msg;
        std::vector<int> bin_text;
        std::vector<int> hamming_encoded;
        std::vector<int> interleaved;
    };
    
    struct flags
    {
        std::atomic_bool exit = false;
        std::atomic_bool msg_r = false;
        std::atomic_bool bin_msg_r = false;
        std::atomic_bool ham_msg_r = false;
        std::atomic_bool inter_msg_r = false;
    };
    
    data d;
    flags f;
};
