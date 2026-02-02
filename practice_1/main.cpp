#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <bitset>

using namespace std;

bitset<8> decimal_to_binary(int decimal) {
    bitset<8> binary;
    binary.reset();
    int i = 0;

    while (decimal != 0 && i < 8){
        binary[i] = decimal % 2;
        decimal /= 2;
        ++i;
    }
    return binary;
}

int binary_to_decimal(bitset<8> binary) {
    int decimal = 0;
    for (int i = 7; i >= 0; --i) {
        decimal += binary[i] * pow(2, i);
    }
    return decimal;
}

int main(){
    vector<bitset<8>> bin_text;
    string orig_text;
    string decoded_text;
    while(true) {
        cout << "Enter your text(between 30 and 100 symbols): ";
        getline(cin, orig_text);
        if (orig_text.size() <= 100 && orig_text.size() >= 30) break;
    }

    for (char c : orig_text) {
        int dec_ascii = int(c);
        bin_text.push_back(decimal_to_binary(dec_ascii));
    }

    cout << "Encoded Text: ";
    for (auto e : bin_text) cout << "\n" << e;
    cout << "\n";

    for (size_t i = 0; i < bin_text.size(); ++i) {
        int decoded_decim = binary_to_decimal(bin_text[i]);
        decoded_text += char(decoded_decim);
    }

    cout << "Decoded Text: " << decoded_text << "\n";
}
