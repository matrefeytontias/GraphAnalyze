#define GLFW_DLL
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.

#include <iostream>
#include <stdexcept>
#include <string>

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// Define these only in *one* .cpp file.
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "utils.h"

#include <math.h>

using namespace ImGui;

void setupImGuiStyle();

static void glfw_error_callback(int error, const char *description)
{
    trace("Error " << error << " : " << description);
}

void keyCallback(GLFWwindow *window, int key, int scanCode, int action, int mods)
{
    ImGui_ImplGlfw_KeyCallback(window, key, scanCode, action, mods);
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS && !ImGui::GetIO().WantCaptureKeyboard)
        glfwSetWindowShouldClose(window, true);
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

int _main(int, char *argv[])
{
    setwd(argv);

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        trace("Couldn't initialize GLFW");
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(1280, 720, "Graph&Analyze", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1); // Enable vsync

    // Setup ImGui binding
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    ImGui_ImplGlfwGL3_Init(window, true);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    // Setup style
    setupImGuiStyle();
    // ImGui::StyleColorsClassic();

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    trace("Entering drawing loop");

    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplGlfwGL3_NewFrame();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        float startPos = ImGui::GetWindowSize().x *0.4f;
        ImGui::SetCursorPosX(startPos + 100);
        ImGui::Text("General Tools");
        ImGui::SetCursorPosX(startPos);
        if(ImGui::Button("open",ImVec2(50,30)))
        {
                //todo
        }
        ImGui::SameLine();
        if(ImGui::Button("close",ImVec2(50,30)))
        {
                //todo
        }
        ImGui::SameLine();
        if(ImGui::Button("Undo",ImVec2(50,30)))
        {
                //todo
        }
        ImGui::SameLine();
        if(ImGui::Button("Redo",ImVec2(50,30)))
        {
                //todo
        }
        Separator();
        ImGui::NewLine();
        float lines[10];
        for (int i = 0;  i< 10; i++)
            lines[i] = (float)(exp(i));
        ImGui::BeginGroup();
        ImGui::Text("Mathematical Tools");
        ImGui::NewLine();
        if(ImGui::Button("op1",ImVec2(50,30)))
        {
                //todo
        }
        if(ImGui::Button("op2",ImVec2(50,30)))
        {
                //todo
        }

        if(ImGui::Button("op3",ImVec2(50,30)))
        {
                //todo
        }
        ImGui::EndGroup();
        ImGui::SameLine();
        ImGui::BeginGroup();
        int startPosGraph = GetCursorPosX();
        char input[50] = {};
        float minX = 0.0f;
        float maxX = 10.0f;
        PushItemWidth((ImGui::GetWindowWidth() - startPosGraph -20)-100);
        ImGui::InputText("Formule",input,sizeof(input));
        PushItemWidth((ImGui::GetWindowWidth() - startPosGraph -20)/3);
        ImGui::InputFloat("Min x",&minX,0.01f);
        ImGui::SameLine();
        ImGui::InputFloat("Max x",&maxX,0.01f);
        PopItemWidth();
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PlotLines("graph",lines,10,0,NULL,FLT_MAX,FLT_MAX, ImVec2((ImGui::GetWindowWidth() - startPosGraph - 60), (ImGui::GetWindowHeight()-GetCursorPosY()-100)),sizeof(float));
        float tabSize = (ImGui::GetWindowWidth() - startPosGraph -20)/2-50;
        PushStyleVar(ImGuiStyleVar_FrameBorderSize, .1f);
        if(ImGui::Button("Tab1",ImVec2(tabSize,30)))
        {
                //todo
        }
        ImGui::SameLine(0.0f,0.0f);
        if(ImGui::Button("Tab2",ImVec2(tabSize,30)))
        {
                //todo
        }
        ImGui::SameLine(0.0f,0.0f);
        if(ImGui::Button("+",ImVec2(50,30)))
        {
                //todo
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
        ImGui::EndGroup();


        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    trace("Exiting drawing loop");

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;
}

int main(int argc, char *argv[])
{
    try
    {
        return _main(argc, argv);
    }
    catch(std::exception &e)
    {
        std::cerr << e.what();
        throw;
    }
}

void setupImGuiStyle()
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    io.Fonts->AddFontFromFileTTF("fonts/OpenSans-Light.ttf", 20);
    io.Fonts->AddFontFromFileTTF("fonts/OpenSans-Regular.ttf", 20);
    io.Fonts->AddFontFromFileTTF("fonts/OpenSans-Light.ttf", 40);
    io.Fonts->AddFontFromFileTTF("fonts/OpenSans-Regular.ttf", 16);
    io.Fonts->AddFontFromFileTTF("fonts/OpenSans-Bold.ttf", 16);
    io.Fonts->Build();

    ImGuiStyle * style = &ImGui::GetStyle();

    style->WindowPadding = ImVec2(20, 25);
    style->WindowRounding = 10.0f;
    style->WindowMinSize = ImVec2(500,400);
    style->FramePadding = ImVec2(5, 5);
    style->FrameRounding = 5.0f;


    style->ItemSpacing = ImVec2(12, 8);
    style->ItemInnerSpacing = ImVec2(8, 6);
    style->IndentSpacing = 25.0f;

    style->Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.f);
    style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.0f, 0.f, 0.f, 0.5f);

    style->Colors[ImGuiCol_WindowBg] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);;
    style->Colors[ImGuiCol_PopupBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.4f);
    style->Colors[ImGuiCol_Border] = ImVec4(.0f, .0f, .0f, 1.f);
    style->Colors[ImGuiCol_FrameBg] = ImVec4(1.0f, 1.f, 1.f, 1.00f);
  	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(1.f, 1.f, 1.f, 1.00f);
  	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(1.f, 1.f, 1.f, 1.00f);

    style->Colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
    style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 1.f, 0.f, 0.3f);
    style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.f, 1.0f, 0.0f, 1.00f);
    style->Colors[ImGuiCol_MenuBarBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.47f);

    style->Colors[ImGuiCol_Button] = ImVec4(.6f, .6f, .6f, 0.7);
    style->Colors[ImGuiCol_ButtonHovered] = ImVec4(.8f, .8f, .8f, 1.f);
    style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);

    style->Colors[ImGuiCol_Header] = ImVec4(0.25f, 1.00f, 0.00f, 0.76f);
    style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 1.00f, 0.00f, 0.86f);
    style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_Column] = ImVec4(0.00f, 0.00f, 0.00f, 0.32f);
    style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.25f, 1.00f, 0.00f, 0.78f);
    style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);

    style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.25f, 1.00f, 0.00f, 0.5f);
    style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);

    style->Colors[ImGuiCol_PlotLines] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
}
