#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "dpad.hpp"

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int, char**) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImFontConfig font_cfg;
    font_cfg.SizePixels = 26.0f;
    auto* default_font = io.Fonts->AddFontDefault();
    auto* big_font = io.Fonts->AddFontDefault(&font_cfg);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    int display_w, display_h;
    ImVec4 clear_color = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);

    const char* headers[] = { "Slow", "Medium", "Fast" };
    static std::string output;
    auto formatDirection = [](const char* speedLabel, int dir) -> std::string {
        if (!dir) return {};
        std::string result = speedLabel;
        switch (dir) {
            case 1: result += " Up";    break;
            case 2: result += " Down";  break;
            case 3: result += " Left";  break;
            case 4: result += " Right"; break;
            default: break;
        }
        return result;
    };

    float r[3];
    float radius[3] = { 35.f, 25.f, 25.f };
    bool ribbon[3] = { true, true, false };
    float spacer = 2.f;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        r[0] = radius[0];
        r[1] = radius[1] + r[0];
        r[2] = radius[2] + r[1];
        
        struct DPadConfig {
            const char* id;
            ImVec4 color;
            float r0, r1;
            bool* ribbon;
        };

        DPadConfig dpads[] = {
            { "dpad_fast",   ImVec4(0.8f, 0.1f, 0.1f, 1.f), r[1], r[2], &ribbon[2] },
            { "dpad_medium", ImVec4(0.9f, 0.9f, 0.f, 1.f),  r[0], r[1], &ribbon[1] },
            { "dpad_slow",   ImVec4(0.1f, 0.8f, 0.1f, 1.f), 0.f,  r[0], &ribbon[0] }
        };

        ImGui::Begin("D-Pad demo");
        ImVec2 center = ImGui::GetCursorScreenPos() + ImVec2(r[2], r[2]);

        ImGui::ImGuiDPadDirection dirs[3];
        for (int i = 0; i < 3; ++i) {
            ImGui::PushStyleColor(ImGuiCol_Button, dpads[i].color);
            dirs[i] =   i ? ImGui::ImGuiDPad(dpads[i].id, dpads[i].r0, dpads[i].r1, spacer, *dpads[i].ribbon, center) :
                            ImGui::ImGuiDPad(dpads[i].id, dpads[i].r0, dpads[i].r1, spacer, *dpads[i].ribbon);
            ImGui::PopStyleColor();
        }
        ImGui::End();

        for (int i = 0; i < 3; ++i) {
            std::string dirStr = formatDirection(headers[2 - i], dirs[i]);
            if (!dirStr.empty()) {
                output = dirStr;
                break;
            }
        }

        ImGui::Begin("Output");
        ImGui::Text("Direction: %s", output.c_str());
        ImGui::End();

        // Debug
        for (int i = 0; i < 3; i++) {
            if (ImGui::CollapsingHeader(headers[i], ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::PushID(i);
                ImGui::SliderFloat("##", &radius[i], 1.f, 50.f, "Radius %3.f");
                ImGui::Checkbox("Ribbon", &ribbon[i]);
                ImGui::PopID();
            }
        }
        ImGui::SliderFloat("Spacer", &spacer, 0.f, 10.f);

        ImGui::Render();
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui::DestroyContext();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
