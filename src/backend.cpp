#include "backend.hpp"

#include "funcs.hpp"
#include "fft.hpp"

void run_backend(sharedData &sd)
{
    std::string orig_text;
    std::string decoded_text;
    
    while (!sd.f.exit)
    {
        if (sd.f.msg_r == true)
        {
            int offset = 0;
            sd.d.bin_text.resize(sd.d.s_msg.size() * 8);
            for (char c : sd.d.s_msg)
            {
                auto bits = bitsetToVec(decimal_to_binary(int(c)));
                for (size_t b = 0; b < bits.size(); ++b)
                    sd.d.bin_text[offset + b] = bits[b];
                offset += 8;
            }
            sd.f.msg_r = false;
            sd.f.bin_msg_r = true;

            sd.d.hamming_encoded = hammingEncode(sd.d.bin_text);
            sd.f.ham_msg_r = true;
        }

    }

    

    // std::vector<int> interleaved = interleave(hamming_encoded);
    // printBits(interleaved, "Interleaved bits", 12);
    // std::cout << "\n";

    // std::vector<std::complex<float>> symbols = mod_qpsk_3gpp(interleaved);
    // for (size_t i = 0; i < symbols.size(); ++i)
    //     std::cout << symbols[i] << " ";
    
    // std::cout << "\n";

    // int delta_rs = 0;
    // std::cout << "Enter period (or step) of placement of reference subcarriers: ";
    // std::cin >> delta_rs;

    // float C = 0.0;
    // std::cout << "C param, part of zeros: ";
    // std::cin >> C;

    // int N_rs = std::floor(symbols.size() / delta_rs);
    // int N_z  = (N_rs + symbols.size()) * C;

    // int frame_size = symbols.size() + 2 * N_z;

    // while (true) {
    //     size_t half = frame_size / 2;
    //     int cnt = 0;
    //     for (int i = 0; i < frame_size; ++i) {
    //         bool in_zero  = (i >= (int)(half - N_z) && i <= (int)(half + N_z));
    //         bool in_pilot = (!in_zero && i % delta_rs == 0);
    //         if (!in_zero && !in_pilot) ++cnt;
    //     }
    //     if (cnt >= (int)symbols.size()) break;
    //     ++frame_size;
    // }

    // std::vector<bool> is_zeros(frame_size, false);
    // std::vector<bool> is_pilot(frame_size, false);
    // std::vector<bool> is_data(frame_size, false);
    // size_t zeros_half = frame_size / 2;

    // for (size_t i = 0; i < is_zeros.size(); ++i)
    //     if (i >= zeros_half - N_z && i <= zeros_half + N_z)
    //         is_zeros[i] = true;

    // for (size_t i = 0; i < is_pilot.size(); ++i)
    //     if (i % delta_rs == 0 && !is_zeros[i])
    //         is_pilot[i] = true;

    // std::cout << "Pilots" << "\n";
    // for (const auto &e : is_pilot)
    //     std::cout << e << " ";

    // std::cout << "Zeros" << "\n";
    // for (const auto &e : is_zeros)
    //     std::cout << e << " ";

    // for (size_t i = 0; i < is_zeros.size(); ++i)
    //     if (!is_pilot[i] and !is_zeros[i])
    //         is_data[i] = true;

    // std::vector<std::complex<float>> multi_data = channel_multiplexer(is_zeros, is_pilot, is_data, symbols);

    // std::cout << "Multi Data:\n";
    // for (size_t i = 0; i < multi_data.size(); ++i)
    //     std::cout << multi_data[i] << " ";
    
    // fft dpf(multi_data.size());
    // dpf.executeBackward(multi_data);

    // int cp_len = 0;
    // std::cout << "Enter cyclic prefex length: ";
    // std::cin >> cp_len;

    // std::vector<std::complex<float>> tx = add_cp(multi_data, cp_len);
    // for (size_t i = 0; i < tx.size(); ++i)
    //     std::cout << tx[i] << " ";


    /*
    Transmission medium
    */


    // std::vector<int> dem_bits = demod_qpsk_3gpp(symbols);
    // printBits(dem_bits, "Demapped bits", 12);
    // std::cout << "\n";

    // std::vector<std::vector<int>> deinterleaved = deinterleave(dem_bits, hamming_encoded.size(), hamming_encoded[0].size());
    // std::cout << "Deinterleaved bits:\n";
    // for (auto &word : deinterleaved)
    // {
    //     for (int b : word)
    //         std::cout << b;
    //     std::cout << " ";
    // }
    // std::cout << "\n";

    // std::string hamming_decoded = "";   
    // for (auto &word : deinterleaved)
    // {
    //     auto data_bits = hammingDecode(word);
    //     auto bs = vecToBitset(data_bits);
    //     hamming_decoded += char(binary_to_decimal(bs));
    // }
    // std::cout << "After Hamming decode: " << hamming_decoded << "\n";
}