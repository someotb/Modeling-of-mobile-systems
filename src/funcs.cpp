#include "funcs.hpp"

#include <algorithm>
#include <random>

#define M_PIf 3.14159265358979323846f

std::bitset<8> decimal_to_binary(int decimal)
{
    std::bitset<8> binary;
    binary.reset();
    int i = 0;

    while (decimal != 0 && i < 8)
    {
        binary[i] = decimal % 2;
        decimal /= 2;
        ++i;
    }
    return binary;
}

int binary_to_decimal(std::bitset<8> binary)
{
    int decimal = 0;
    for (int i = 7; i >= 0; --i)
    {
        decimal += binary[i] * pow(2, i);
    }
    return decimal;
}

std::vector<int> bitsetToVec(std::bitset<8> bs)
{
    std::vector<int> bits(8);
    for (int i = 0; i < 8; i++)
        bits[i] = bs[i];
    return bits;
}

std::bitset<8> vecToBitset(const std::vector<int> &bits)
{
    std::bitset<8> bs;
    for (int i = 0; i < 8; i++)
        bs[i] = bits[i];
    return bs;
}

bool isPowerOfTwo(int n)
{
    return n > 0 && (n & (n - 1)) == 0;
}

std::vector<int> hammingEncode(std::vector<int> data)
{
    int m = data.size();

    int r = 0;
    while ((1 << r) < m + r + 1)
        r++;

    int n = m + r;
    std::vector<int> code(n + 1, 0);

    int dataIdx = 0;
    for (int i = 1; i <= n; i++)
        if (!isPowerOfTwo(i))
            code[i] = data[dataIdx++];

    for (int i = 0; i < r; i++)
    {
        int p = 1 << i;
        int xorSum = 0;
        for (int j = p; j <= n; j++)
            if (j & p)
                xorSum ^= code[j];
        code[p] = xorSum;
    }

    return std::vector<int>(code.begin() + 1, code.end());
}

std::vector<int> hammingDecode(std::vector<int> received, sharedData &sd)
{
    int n = received.size();
    std::vector<int> code(n + 1);
    for (int i = 0; i < n; i++)
        code[i + 1] = received[i];

    int errorPos = 0;
    int r = (int)std::log2(n) + 1;
    for (int i = 0; i < r; i++)
    {
        int p = 1 << i;
        if (p > n)
            break;
        int xorSum = 0;
        for (int j = p; j <= n; j++)
            if (j & p)
                xorSum ^= code[j];
        if (xorSum)
            errorPos |= p;
    }

    if (errorPos != 0)
    {
        sd.d.ham.errs_pos.push_back(errorPos);
        code[errorPos] ^= 1;
    }

    std::vector<int> data;
    for (int i = 1; i <= n; i++)
        if (!isPowerOfTwo(i))
            data.push_back(code[i]);
    return data;
}

std::vector<int> interleave(std::vector<std::vector<int>> &words)
{
    std::vector<int> result;
    int rows = words.size();
    int cols = words[0].size();

    for (int col = 0; col < cols; ++col)
        for (int row = 0; row < rows; ++row)
            result.push_back(words[row][col]);
    return result;
}

std::vector<std::vector<int>> deinterleave(std::vector<int> &data, int rows, int cols)
{
    std::vector<std::vector<int>> words(rows, std::vector<int>(cols));
    int idx = 0;
    for (int col = 0; col < cols; col++)
        for (int row = 0; row < rows; row++)
            words[row][col] = data[idx++];
    return words;
}

std::vector<std::complex<float>> mod_qpsk_3gpp(const std::vector<int> &bits)
{
    std::vector<std::complex<float>> symbols(bits.size() / 2);
    for (size_t i = 0; i < bits.size() / 2; ++i)
    {
        float b0 = (1.0f - 2.0f * bits[2 * i]);
        float b1 = (1.0f - 2.0f * bits[2 * i + 1]);
        symbols[i] = std::complex(b0, b1) / std::sqrt(2.0f);
    }
    return symbols;
}

std::vector<int> demod_qpsk_3gpp(const std::vector<std::complex<float>> &symbols)
{
    std::vector<int> bits(symbols.size() * 2);
    for (size_t i = 0; i < symbols.size(); ++i)
    {
        bits[2 * i] = symbols[i].real() > 0 ? 0 : 1;
        bits[2 * i + 1] = symbols[i].imag() > 0 ? 0 : 1;
    }

    return bits;
}

std::vector<std::complex<float>> channel_multiplexer(const std::vector<bool> &is_zeros, const std::vector<bool> &is_pilot, const std::vector<bool> &is_data, const std::vector<std::complex<float>> &data)
{
    std::vector<std::complex<float>> signal(is_zeros.size());
    int data_offset = 0;
    for (size_t i = 0; i < signal.size(); ++i)
    {
        if (is_zeros[i])
            signal[i] = std::complex(0.0f, 0.0f);
        else if (is_pilot[i])
            signal[i] = std::complex(1.0f - 2.0f * 1.0f, 1.0f - 2.0f * 1.0f) / std::sqrt(2.0f);
        else if (is_data[i])
        {
            signal[i] = data[data_offset];
            data_offset++;
        }
    }

    return signal;
}

std::vector<std::complex<float>> add_cp(const std::vector<std::complex<float>> &data, float size_cp)
{
    int cp_len = data.size() * size_cp;
    std::vector<std::complex<float>> new_data(data.size() + cp_len);

    for (size_t i = 0; i < cp_len; ++i)
    {
        int end_idx = data.size() - cp_len + i;
        new_data[i] = data[end_idx];
    }

    for (size_t i = 0; i < data.size(); ++i)
    {
        int start_idx = i + cp_len;
        new_data[start_idx] = data[i];
    }

    return new_data;
}

std::vector<std::complex<float>> add_multipath(std::vector<std::complex<float>> &tx, sharedData &sd)
{
    float c = 3e8;
    float Ts = 1 / sd.p.s.bandwidth;

    std::vector<int> beam_len(sd.p.m.cnt_beam);
    for (size_t i = 0; i < beam_len.size(); ++i)
        beam_len[i] = 10 + rand() % (sd.p.m.path_len - 10 + 1);


    auto min_beam_it = std::ranges::min_element(beam_len);
    auto index_beam_it = std::distance(beam_len.begin(), min_beam_it);
    int first_beam = beam_len[0];
    beam_len[0] = *min_beam_it;
    beam_len[index_beam_it] = first_beam;

    std::vector<int> latency(sd.p.m.cnt_beam);
    for (size_t i = 0; i < latency.size(); ++i)
        latency[i] = std::round((beam_len[i] - beam_len[0]) / (c * Ts));

    std::vector<float> attenuationCoeffs(beam_len.size(), 0);
    for (size_t i = 0; i < attenuationCoeffs.size(); ++i)
        attenuationCoeffs[i] = c / (4 * M_PIf * beam_len[i] * sd.p.s.carr_freq);

    auto max_latency = std::ranges::max(latency);

    std::vector<std::complex<float>> tx_multipath(tx.size() + max_latency, {0.0f, 0.0f});
    for (size_t k = 0; k < tx_multipath.size(); ++k)
    {
        for (size_t i = 0; i < latency.size(); ++i)
        {
            if (k >= latency[i] && k - latency[i] < tx.size())
                tx_multipath[k] += attenuationCoeffs[i] * tx[k - latency[i]];
        }
    }
    return tx_multipath;
}

std::vector<std::complex<float>> add_wgn(std::vector<std::complex<float>> &tx, sharedData &sd)
{
    float psd_linear = std::pow(10.0f, sd.p.w.psd / 10.0f);
    float sigma = std::sqrt(psd_linear * sd.p.s.bandwidth / 2.0f);
    std::mt19937 gen(std::random_device{}());
    std::normal_distribution<float> dist(0.0f, sigma);

    std::vector<std::complex<float>> tx_noisy(tx.size());
    for (size_t i = 0; i < tx_noisy.size(); ++i)
        tx_noisy[i] = tx[i] + std::complex<float>(dist(gen), dist(gen));

    return tx_noisy;
}
