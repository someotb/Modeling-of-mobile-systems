#include "backend.hpp"
#include "funcs.hpp"
#include "fft.hpp"

void run_backend(sharedData &sd)
{
    std::string orig_text;
    std::string decoded_text;
    std::vector<std::bitset<8>> bin_text;
    std::vector<std::vector<int>> hamming_encoded;
    std::vector<int> interleaved;
    std::vector<std::complex<float>> selected;
    std::vector<std::complex<float>> symbols;
    std::vector<std::complex<float>> multi_data;
    std::vector<std::complex<float>> tx;
    std::vector<std::complex<float>> tx_noisy;
    std::vector<std::complex<float>> tx_multi_path;
    std::vector<std::complex<float>> tx_multi_path_noisy;
    std::vector<std::complex<float>> rx_rm_cp;
    std::vector<std::complex<float>> rx_rm_cp_zeros;

    std::vector<int> dem_bits;
    std::vector<std::vector<int>> deinterleaved;
    
    while (!sd.f.a.exit)
    {
        if (sd.f.s.msg_r)
        {
            bin_text.clear();
            hamming_encoded.clear();
            interleaved.clear();
            symbols.clear();
            multi_data.clear();
            for (char c : sd.d.s_msg)
            {
                int dec_ascii = int(c);
                bin_text.push_back(decimal_to_binary(dec_ascii));
            }

            for (auto &bs : bin_text)
            {
                auto bits = bitsetToVec(bs);
                auto encoded = hammingEncode(bits);
                hamming_encoded.push_back(encoded);
            }
            
            interleaved = interleave(hamming_encoded);
            symbols = mod_qpsk_3gpp(interleaved);
            int N_rs = std::floor(symbols.size() / sd.p.o.pilots_step);
            int N_z  = (N_rs + symbols.size()) * sd.p.o.zero_guard; 
            int frame_size = symbols.size() + 2 * N_z;

            while (true)
            {
                size_t half = frame_size / 2;
                int cnt = 0;
                for (int i = 0; i < frame_size; ++i) {
                    bool in_zero  = (i >= (int)(half - N_z) && i <= (int)(half + N_z));
                    bool in_pilot = (!in_zero && i % sd.p.o.pilots_step == 0);
                    if (!in_zero && !in_pilot) ++cnt;
                }
                if (cnt >= (int)symbols.size()) break;
                ++frame_size;
            }

            size_t zeros_half = frame_size / 2;
            std::vector<bool> is_zeros(frame_size, false);
            std::vector<bool> is_pilot(frame_size, false);
            std::vector<bool> is_data(frame_size, false);

            for (size_t i = 0; i < is_zeros.size(); ++i)
                if (i >= zeros_half - N_z && i <= zeros_half + N_z)
                    is_zeros[i] = true;
        
            for (size_t i = 0; i < is_pilot.size(); ++i)
                if (i % sd.p.o.pilots_step == 0 && !is_zeros[i])
                    is_pilot[i] = true;
            
            for (size_t i = 0; i < is_zeros.size(); ++i)
                if (!is_pilot[i] and !is_zeros[i])
                    is_data[i] = true;

            multi_data = channel_multiplexer(is_zeros, is_pilot, is_data, symbols);
            fft dpf(multi_data.size());
            dpf.executeBackward(multi_data);
            tx = add_cp(multi_data, sd.p.o.cp_len);

            // Transmission medium

            tx_noisy = add_wgn(tx, sd);
            tx_multi_path = add_multipath(tx, sd);
            tx_multi_path_noisy = add_wgn(tx_multi_path, sd);

            // Transmission medium

            switch (sd.f.s.view_mode)
            {
                case ViewMode::Raw:
                    selected = tx;
                    break;
                case ViewMode::Noisy:
                    selected = tx_noisy;
                    break;
                case ViewMode::Multipath:
                    selected = tx_multi_path;
                    break;
                case ViewMode::MultipathNoisy:
                    selected = tx_multi_path_noisy;
                    break;
            }

            rx_rm_cp = rm_cp(selected, multi_data.size(), sd.p.o.cp_len);
            dpf.executeForward(rx_rm_cp);
            rx_rm_cp_zeros = rm_zeros(rx_rm_cp, is_zeros);

            std::lock_guard<std::mutex> lock(sd.s.data_mutex);
            sd.d.gui_output = rx_rm_cp_zeros;

            dem_bits = demod_qpsk_3gpp(symbols);
            deinterleaved = deinterleave(dem_bits, hamming_encoded.size(), hamming_encoded[0].size());
            sd.d.r_msg = "";
            sd.d.ham.errs_pos.clear();

            for (auto &word : deinterleaved)
            {
                auto data_bits = hammingDecode(word, sd);
                auto bs = vecToBitset(data_bits);
                sd.d.r_msg += char(binary_to_decimal(bs));
            }
        }
    }
}