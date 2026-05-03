#pragma once

#include <string>
#include <atomic>
#include <vector>
#include <bitset>
#include <complex>
#include <mutex>

enum class ViewMode { Raw, Multipath, Noisy };

struct sharedData
{
    struct data
    {
        char c_msg[101] = "Hello World! Hello Heaven! Hello Paradise! Hello Hello!";
        std::string s_msg = "Hello World! Hello Heaven! Hello Paradise! Hello Hello!";
        std::string r_msg = "";
        std::vector<std::complex<float>> gui_output;

        struct hamming
        {
            std::vector<int> errs_pos;
        };
        
        hamming ham;
    };
    
    struct flags
    {
        struct app
        {
            std::atomic_bool exit{false};
            std::atomic_bool run_gui{false};
        };

        struct states
        {
            std::atomic_bool msg_r{false};
            ViewMode view_mode = ViewMode::Raw;
        };
        
        app a;
        states s;
    };

    struct params
    {
        struct signal
        {
            float bandwidth = 8e6;
            float carr_freq = 2.4e9;
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
            int path_len = 500;
        };

        struct wgn
        {
            int psd = -120;
        };
        

        signal s;
        ofdm o;
        multipath m;
        wgn w;
    };

    struct sync
    {
        std::mutex data_mutex;
    };
    
    
    data d;
    flags f;
    params p;
    sync s;
};
