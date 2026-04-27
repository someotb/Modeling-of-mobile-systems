#include "funcs.hpp"
#include "fft.hpp"
#include "sharedData.hpp"

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include "implot.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
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
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow(
        "MMS",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");

    bool running = true;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                running = false;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Твоё окно
        ImGui::Begin("MMS Panel");
        ImGui::Text("Hello!");
        // сюда будешь добавлять графики, кнопки и тд
        ImGui::End();

        ImGui::Render();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main()
{
    sharedData sh_data;
    std::thread backend_t(run_backend, std::ref(sh_data));
    std::thread gui_t(run_gui, std::ref(sh_data));

    if (backend_t.joinable())
        backend_t.join();
    if (gui_t.joinable())
        gui_t.join();

    return 0;
}
