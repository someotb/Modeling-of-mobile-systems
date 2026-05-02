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


        if (ImGui::Begin("Text"))
        {
            ImGui::InputText("Msg", sd.d.c_msg, 101);
            sd.d.s_msg = sd.d.c_msg;
            
            int len = strlen(sd.d.c_msg);
            
            if (len < 30)
                ImGui::TextColored({1,0.3f,0.3f,1}, "Min 30 symbols (%d/30)", len);
            else
                ImGui::TextColored({0.3f,1,0.3f,1}, "%d symbols", len);
            
            ImGui::BeginDisabled(len < 30 || len > 100);
            if (ImGui::Button("Send"))
                sd.f.msg_r = true;

            ImGui::EndDisabled();

            ImGui::Text("Decoded text: %s", sd.d.r_msg.c_str());

            ImGui::Text("Errors positions");
            for (size_t i = 0; i < sd.d.ham.errs_pos.size(); ++i)
                ImGui::Text("Error pos: %d", sd.d.ham.errs_pos[i]);

            ImGui::End();
        }

        if (ImGui::Begin("PlotLine"))
        {
            if (ImPlot::BeginPlot("I/Q", ImVec2(ImGui::GetContentRegionAvail())))
            {
                float* raw = reinterpret_cast<float*>(sd.d.tx.data());
                int n = sd.d.tx.size();
                int stride = sizeof(std::complex<float>);

                ImPlot::PlotLine("I", raw, n, 1.0, 0.0, 0, 0, stride);
                ImPlot::PlotLine("Q", raw + 1, n, 1.0, 0.0, 0, 0, stride);

                ImPlot::EndPlot();
            }
            ImGui::End();
        }

        if (ImGui::Begin("PlotScatter"))
        {
            if (ImPlot::BeginPlot("Constelation Diagramm", ImVec2(ImGui::GetContentRegionAvail())))
            {
                float* raw = reinterpret_cast<float*>(sd.d.tx.data());
                int n = sd.d.tx.size();
                int stride = sizeof(std::complex<float>);

                ImPlot::PlotScatter("I/Q", raw, raw + 1, n, 0, 0, stride);

                ImPlot::EndPlot();
            }
            ImGui::End();
        }

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

    sd.f.exit = true;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return;
}