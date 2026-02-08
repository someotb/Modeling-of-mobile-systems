#include "funcs.hpp"

std::bitset<8> decimal_to_binary(int decimal) {
    std::bitset<8> binary;
    binary.reset();
    int i = 0;

    while (decimal != 0 && i < 8){
        binary[i] = decimal % 2;
        decimal /= 2;
        ++i;
    }
    return binary;
}

int binary_to_decimal(std::bitset<8> binary) {
    int decimal = 0;
    for (int i = 7; i >= 0; --i) {
        decimal += binary[i] * pow(2, i);
    }
    return decimal;
}
