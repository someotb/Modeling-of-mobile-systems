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
    };
    
    struct flags
    {
        std::atomic_bool exit = false;
        std::atomic_bool msg_r = false;
        std::atomic_bool bin_msg_r = false;
    };
    
    data d;
    flags f;
};
