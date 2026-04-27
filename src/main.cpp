#include "funcs.hpp"

int main()
{
    std::vector<std::bitset<8>> bin_text;
    std::string orig_text;
    std::string decoded_text;
    while (true)
    {
        std::cout << "Enter your text(between 30 and 100 symbols): ";
        std::getline(std::cin, orig_text);
        if (orig_text.size() <= 100 && orig_text.size() >= 30)
            break;
    }

    for (char c : orig_text)
    {
        int dec_ascii = int(c);
        bin_text.push_back(decimal_to_binary(dec_ascii));
    }

    std::vector<std::vector<int>> hamming_encoded;

    for (auto &bs : bin_text)
    {
        auto bits = bitsetToVec(bs);
        auto encoded = hammingEncode(bits);
        hamming_encoded.push_back(encoded);
    }

    std::cout << "Hamming encoded:\n";
    for (auto &word : hamming_encoded)
    {
        for (int b : word)
            std::cout << b;
        std::cout << " ";
    }
    std::cout << "\n";

    std::vector<int> interleaved = interleave(hamming_encoded);
    printBits(interleaved, "Interleaved bits", 12);
    std::cout << "\n";

    std::vector<std::vector<int>> deinterleaved = deinterleave(interleaved, hamming_encoded.size(), hamming_encoded[0].size());
    std::cout << "Deinterleaved bits:\n";
    for (auto &word : deinterleaved)
    {
        for (int b : word)
            std::cout << b;
        std::cout << " ";
    }
    std::cout << "\n";

    std::string hamming_decoded = "";   
    for (auto &word : deinterleaved)
    {
        auto data_bits = hammingDecode(word);
        auto bs = vecToBitset(data_bits);
        hamming_decoded += char(binary_to_decimal(bs));
    }
    std::cout << "After Hamming decode: " << hamming_decoded << "\n";
}
