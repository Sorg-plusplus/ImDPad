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

    auto formatDirection = [](int dir) -> std::string {
        if (!dir) return {};
        std::string result;
        switch (dir) {
            case 1: result = "Up";    break;
            case 2: result = "Down";  break;
            case 3: result = "Left";  break;
            case 4: result = "Right"; break;
            default: break;
        }
        return result;
    };

    float r0 = 60.f, r1 = 85.f;
    bool ribbon = true;
    float spacer = 0.f;
    std::string output;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("D-Pad demo");
        ImGui::ImGuiDPadDirection dir = ImGui::ImGuiDPad("dpad", (r0 < spacer) ? spacer : r0 - spacer, r1 - spacer, spacer, ribbon);
        ImGui::End();

        std::string dirStr = formatDirection(dir);
        if (!dirStr.empty())
            output = dirStr;

        ImGui::Begin("Output");
        ImGui::Text("Direction: %s", output.c_str());
        ImGui::End();

        // Debug
        if (ImGui::SliderFloat("##0", &r0, 0.f, 80.f, "Radius %3.f"))
            if (r0 > r1) r0 = r1 - 2.f * spacer;
        if (ImGui::SliderFloat("##1", &r1, 1.f, 100.f, "Radius %3.f"))
            if (r1 < r0) r1 = r0 + 2.f * spacer;
        ImGui::Checkbox("Ribbon", &ribbon);
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
