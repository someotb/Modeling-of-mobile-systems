#pragma once

#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <bitset>

std::bitset<8> decimal_to_binary(int decimal);
int binary_to_decimal(std::bitset<8> binary);
std::vector<int> bitsetToVec(std::bitset<8> bs);
std::bitset<8> vecToBitset(const std::vector<int> &bits);
bool isPowerOfTwo(int n);
std::vector<int> hammingEncode(std::vector<int> data);
std::vector<int> hammingDecode(std::vector<int> received);