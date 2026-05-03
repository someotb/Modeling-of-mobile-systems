#include "gui.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include <GLFW/glfw3.h>
#include <cstdio>

void run_gui(sharedData &sd)
{
    if (!glfwInit()) return;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow* window = glfwCreateWindow(1280, 720, "MMS", nullptr, nullptr);
    if (!window) return;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui::StyleColorsDark();

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        // Start GUI

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::RadioButton("Raw", (int*)&sd.f.s.view_mode, (int)ViewMode::Raw)){}
                if (ImGui::RadioButton("AGN", (int*)&sd.f.s.view_mode, (int)ViewMode::Noisy)){}
                if (ImGui::RadioButton("Multipath", (int*)&sd.f.s.view_mode, (int)ViewMode::Multipath)){}
                if (ImGui::RadioButton("Multipath+AGN", (int*)&sd.f.s.view_mode, (int)ViewMode::MultipathNoisy)){}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Control Panel"))
            {
                bool mess_r = sd.f.s.msg_r;
                if (ImGui::MenuItem("Send", NULL, &mess_r))
                    sd.f.s.msg_r = mess_r;
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("OFDM params"))
            {
                ImGui::InputInt("Pilots step", &sd.p.o.pilots_step, 1, 10);
                ImGui::InputFloat("CP-to-data ratio", &sd.p.o.cp_len, 0.05, 0.1);
                ImGui::InputFloat("Zero-to-data ratio", &sd.p.o.zero_guard, 0.05, 0.1);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("MultiPath"))
            {
                ImGui::InputInt("Beam count", &sd.p.m.cnt_beam, 1, 10);
                ImGui::InputInt("Max beam length", &sd.p.m.path_len, 1, 10);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("AGN"))
            {
                ImGui::InputInt("PSD", &sd.p.w.psd, 1, 10);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Text Editor"))
            {
                ImGui::InputText("###Msg", sd.d.c_msg, 101);
                sd.d.s_msg = sd.d.c_msg;
                
                int len = strlen(sd.d.c_msg);

                ImGui::SameLine();
                
                if (len < 30)
                {
                    ImGui::TextColored({1,0.3f,0.3f,1}, "[30 <= %d <= 100]", len);
                    sd.d.s_msg = "Hello World! Hello Heaven! Hello Paradise! Hello Hello!";
                }
                else
                    ImGui::TextColored({0.3f,1,0.3f,1}, "[30 <= %d <= 100]", len);
                
                ImGui::EndMenu();
            }

            static bool debug_panel_open = false;
            if (ImGui::BeginMenu("Debug"))
            {
                ImGui::MenuItem("Open Debug Panel", NULL, &debug_panel_open);
                ImGui::EndMenu();
            }

            if (debug_panel_open)
            {
                ImGui::Begin("Debug Panel", &debug_panel_open);
                ImGui::TextColored({0.3f,1,0.3f,1}, "Decoded text: ");
                ImGui::SameLine();
                ImGui::Text("%s", sd.d.r_msg.c_str());

                if (sd.d.ham.errs_pos.empty())
                    ImGui::TextColored({0.3f,1,0.3f,1}, "No errors");
                else
                {
                    ImGui::TextColored({1,0.3f,0.3f,1}, "Errors positions: ");
                    for (size_t i = 0; i < sd.d.ham.errs_pos.size(); ++i)
                    ImGui::TextColored({1,0.3f,0.3f,1}, "Error pos: %d", sd.d.ham.errs_pos[i]);
                }
                ImGui::End();
            }
                
            ImGui::EndMainMenuBar();
        }

        if (ImGui::Begin("PlotLine"))
        {
            if (ImPlot::BeginPlot("I/Q", ImVec2(ImGui::GetContentRegionAvail())))
            {
                std::lock_guard<std::mutex> lock(sd.s.data_mutex);
                float* raw = reinterpret_cast<float*>(sd.d.gui_output.data());
                int n = sd.d.gui_output.size();
                int stride = sizeof(std::complex<float>);

                ImPlot::PlotLine("I", raw, n, 1.0, 0.0, 0, 0, stride);
                ImPlot::PlotLine("Q", raw + 1, n, 1.0, 0.0, 0, 0, stride);

                ImPlot::EndPlot();
            }
        }
        ImGui::End();

        if (ImGui::Begin("PlotScatter"))
        {
            if (ImPlot::BeginPlot("Constelation Diagramm", ImVec2(ImGui::GetContentRegionAvail())))
            {
                std::lock_guard<std::mutex> lock(sd.s.data_mutex);
                float* raw = reinterpret_cast<float*>(sd.d.gui_output.data());
                int n = sd.d.gui_output.size();
                int stride = sizeof(std::complex<float>);

                ImPlot::PlotScatter("I/Q", raw, raw + 1, n, 0, 0, stride);

                ImPlot::EndPlot();
            }
        }
        ImGui::End();

        // End GUI

        ImGui::Render();

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup);
        }
    }

    sd.f.a.exit = true;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return;
}