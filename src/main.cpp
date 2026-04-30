#include "funcs.hpp"
#include "fft.hpp"
#include "sharedData.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <cstdio>
#include <thread>

void run_backend(sharedData &sh_data)
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

    std::vector<std::complex<float>> symbols = mod_qpsk_3gpp(interleaved);
    for (size_t i = 0; i < symbols.size(); ++i)
        std::cout << symbols[i] << " ";
    
    std::cout << "\n";

    int delta_rs = 0;
    std::cout << "Enter period (or step) of placement of reference subcarriers: ";
    std::cin >> delta_rs;

    float C = 0.0;
    std::cout << "C param, part of zeros: ";
    std::cin >> C;

    int N_rs = std::floor(symbols.size() / delta_rs);
    int N_z  = (N_rs + symbols.size()) * C;

    int frame_size = symbols.size() + 2 * N_z;

    while (true) {
        size_t half = frame_size / 2;
        int cnt = 0;
        for (int i = 0; i < frame_size; ++i) {
            bool in_zero  = (i >= (int)(half - N_z) && i <= (int)(half + N_z));
            bool in_pilot = (!in_zero && i % delta_rs == 0);
            if (!in_zero && !in_pilot) ++cnt;
        }
        if (cnt >= (int)symbols.size()) break;
        ++frame_size;
    }

    std::vector<bool> is_zeros(frame_size, false);
    std::vector<bool> is_pilot(frame_size, false);
    std::vector<bool> is_data(frame_size, false);
    size_t zeros_half = frame_size / 2;

    for (size_t i = 0; i < is_zeros.size(); ++i)
        if (i >= zeros_half - N_z && i <= zeros_half + N_z)
            is_zeros[i] = true;

    for (size_t i = 0; i < is_pilot.size(); ++i)
        if (i % delta_rs == 0 && !is_zeros[i])
            is_pilot[i] = true;

    std::cout << "Pilots" << "\n";
    for (const auto &e : is_pilot)
        std::cout << e << " ";

    std::cout << "Zeros" << "\n";
    for (const auto &e : is_zeros)
        std::cout << e << " ";

    for (size_t i = 0; i < is_zeros.size(); ++i)
        if (!is_pilot[i] and !is_zeros[i])
            is_data[i] = true;

    std::vector<std::complex<float>> multi_data = channel_multiplexer(is_zeros, is_pilot, is_data, symbols);

    std::cout << "Multi Data:\n";
    for (size_t i = 0; i < multi_data.size(); ++i)
        std::cout << multi_data[i] << " ";
    
    fft dpf(multi_data.size());
    dpf.executeBackward(multi_data);

    int cp_len = 0;
    std::cout << "Enter cyclic prefex length: ";
    std::cin >> cp_len;

    std::vector<std::complex<float>> tx = add_cp(multi_data, cp_len);
    for (size_t i = 0; i < tx.size(); ++i)
        std::cout << tx[i] << " ";


    /*
    Transmission medium
    */


    std::vector<int> dem_bits = demod_qpsk_3gpp(symbols);
    printBits(dem_bits, "Demapped bits", 12);
    std::cout << "\n";

    std::vector<std::vector<int>> deinterleaved = deinterleave(dem_bits, hamming_encoded.size(), hamming_encoded[0].size());
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

void run_gui(sharedData &sh_data)
{
    if (!glfwInit()) return;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef APPLE
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Создаём окно
    GLFWwindow* window = glfwCreateWindow(1280, 720, "My ImGui App", nullptr, nullptr);
    if (!window) return;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Hello!");
        ImGui::Text("Hello World!");
        ImGui::End();

        ImGui::Render();
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return;
}

int main()
{
    sharedData sh_data;
    std::thread backend_t(run_backend, std::ref(sh_data));
    run_gui(sh_data);

    if (backend_t.joinable())
        backend_t.join();

    return 0;
}
