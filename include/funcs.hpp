#pragma once

#include "sharedData.hpp"

#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <complex>

std::bitset<8> decimal_to_binary(int decimal);
int binary_to_decimal(std::bitset<8> binary);
std::vector<int> bitsetToVec(std::bitset<8> bs);
std::bitset<8> vecToBitset(const std::vector<int> &bits);
bool isPowerOfTwo(int n);
std::vector<int> hammingEncode(std::vector<int> data);
std::vector<int> hammingDecode(std::vector<int> received, sharedData &sd);
void printBits(const std::vector<int> &bits, const std::string &label, int offset);
std::vector<int> interleave(std::vector<std::vector<int>> &words);
std::vector<std::vector<int>> deinterleave(std::vector<int> &data, int rows, int cols);
std::vector<std::complex<float>> mod_qpsk_3gpp(const std::vector<int> &bits);
std::vector<int> demod_qpsk_3gpp(const std::vector<std::complex<float>> &symbols);
std::vector<std::complex<float>> channel_multiplexer(const std::vector<bool> &is_zeros, const std::vector<bool> &is_pilot, const std::vector<bool> &is_data, const std::vector<std::complex<float>> &data);
std::vector<std::complex<float>> add_cp(const std::vector<std::complex<float>> &data, float size_cp);