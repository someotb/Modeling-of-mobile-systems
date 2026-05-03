#pragma once

#include <string>
#include <atomic>
#include <vector>
#include <bitset>
#include <complex>
#include <mutex>

enum class ViewMode { Raw, Noisy, Multipath, MultipathNoisy };

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

        struct debug_info
        {
            int max_latency = 0;
            int cp_samples = 0;
            int N_z = 0;
            int frame_size = 0;
            size_t zeros_half = 0;
            bool is_pilots;
            float h_max = 0.0f;
            int first_pilot = 0;
            int last_pilot = 0;
            int pilot_count;
            size_t rx_size = 0;
            size_t sym_size = 0;
            size_t dem_bits_size = 0;
            size_t expected_size = 0;
        };
        
        hamming h;
        debug_info d;
    };
    
    struct flags
    {
        struct app
        {
            std::atomic_bool exit{false};
            std::atomic_bool run_gui{false};
            std::atomic_bool apply{false};
        };

        struct states
        {
            std::atomic_bool msg_r{false};
            std::atomic_bool regenerate{true};
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
            std::vector<int> beam_len;
        };

        struct wgn
        {
            int psd = -125;
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
    params p_edit;
    sync s;
};
