#define GLFW_DLL
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.

#include <cmath>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#define SHARED 0
#include "mu/muParser.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// Define these only in *one* .cpp file.
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "modules.h"
#include "utils.h"

using namespace std;
using namespace ImGui;

void setupImGuiStyle();
void menu(bool state[]);

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

    GraphAnalyze::GrapherModule grapher;

    bool state[] = {false,false,false,false};
    string name[] = {"module1","module2","module3","module4"};
    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplGlfwGL3_NewFrame();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        menu(state);
        if(state[0])
            grapher.render();
        if(state[1])
            grapher.render();
        if(state[2])
            grapher.render();
        if(state[3])
            grapher.render();
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

void menu(bool state[]){
        ImGui::Begin("module");
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImGui::SetCursorPosX((windowSize.x - ImGui::CalcTextSize("Welcome").x)/2);
        ImGui::Text("Welcome");
        ImVec2 sizeButton = ImVec2(50,30);
        ImVec2 spacing = ImGui::GetStyle().ItemSpacing;
        ImGui::SetCursorPosX((windowSize.x - sizeButton.x *2 -spacing.x)/2);

        if(ImGui::Button("Open",sizeButton)){

        }
        ImGui::SameLine();
        if(ImGui::Button("Exit",sizeButton)){
            exit(0);
        }
        ImGui::Separator();
        ImGui::NewLine();
        ImVec2 textSize = ImGui::CalcTextSize("Plot Graph and show \n information about it");
        ImGui::SetCursorPosX((windowSize.x - textSize.x *2 - spacing.x)/2);
        ImVec2 sizeButtonModule = ImVec2(textSize.x,windowSize.y/10);
        ImGui::SetCursorPosY((windowSize.y - sizeButtonModule.y * 2 - textSize.y*2 - spacing.y * 3)/2);
        ImGui::BeginGroup();  //module list begin
            ImGui::BeginGroup(); //first column
                ImGui::BeginGroup(); //first module
                    if(ImGui::Button("Graph",sizeButtonModule))
                        state[0] = true;
                    ImGui::Text("Plot Graph and show \n information about it");
                ImGui::EndGroup(); //end first module
                ImGui::BeginGroup(); //second module
                    if(ImGui::Button("Module2",sizeButtonModule))
                        state[1]=true;
                    ImGui::Text("Description module2 \n ...");
                ImGui::EndGroup(); //end second module
            ImGui::EndGroup();  //End first column
            ImGui::SameLine();
            ImGui::BeginGroup(); //2nd column
                ImGui::BeginGroup(); //first module
                    if(ImGui::Button("Module3",sizeButtonModule))
                        state[2]=true;
                    ImGui::Text("Description module3 \n ...");
                ImGui::EndGroup(); //end first module
                ImGui::BeginGroup(); //second module
                    if(ImGui::Button("Module4",sizeButtonModule))
                        state[3]=true;
                    ImGui::Text("Description module4 \n ...");
                ImGui::EndGroup(); //end second module
            ImGui::EndGroup();  //End 2nd column
        ImGui::EndGroup(); //module list end
    ImGui::End();
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
    io.Fonts->AddFontFromFileTTF("fonts/OpenSans-Light.ttf", 16);
    io.Fonts->AddFontFromFileTTF("fonts/OpenSans-Regular.ttf", 16);
    io.Fonts->AddFontFromFileTTF("fonts/OpenSans-Light.ttf", 32);
    io.Fonts->AddFontFromFileTTF("fonts/OpenSans-Regular.ttf", 11);
    io.Fonts->AddFontFromFileTTF("fonts/OpenSans-Bold.ttf", 11);
    io.Fonts->Build();

    ImGuiStyle * style = &ImGui::GetStyle();

    style->WindowPadding = ImVec2(15, 15);
    style->WindowRounding = 5.0f;
    style->WindowMinSize = ImVec2(500,400);
    style->FramePadding = ImVec2(5, 5);
    style->FrameRounding = 4.0f;
    style->ItemSpacing = ImVec2(12, 8);
    style->ItemInnerSpacing = ImVec2(8, 6);
    style->IndentSpacing = 25.0f;
    style->ScrollbarSize = 15.0f;
    style->ScrollbarRounding = 9.0f;
    style->GrabMinSize = 5.0f;
    style->GrabRounding = 3.0f;

    style->Colors[ImGuiCol_Text] = ImVec4(0.20f, 0.19f, 0.19f, 1.00f);
    style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.39f, 0.38f, 0.77f);
    style->Colors[ImGuiCol_WindowBg] = ImVec4(0.8f, 0.8f, 0.8f, 1.00f);
    style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.58f);
    style->Colors[ImGuiCol_PopupBg] = ImVec4(0.92f, 0.91f, 0.88f, 0.92f);
    style->Colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 1.f);
    style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
    style->Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 0.98f, 0.95f, 1.00f);
    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.99f, 1.00f, 0.40f, 0.78f);
    style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_TitleBg] = ImVec4(1.00f, 0.98f, 0.95f, 1.00f);
    style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
    style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_MenuBarBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.47f);
    style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(1.00f, 0.98f, 0.95f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 0.00f, 0.00f, 0.21f);
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.90f, 0.91f, 0.00f, 0.78f);
    style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_CheckMark] = ImVec4(0.25f, 1.00f, 0.00f, 0.80f);
    style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 0.00f, 0.00f, 0.14f);
    style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 0.14f);
    style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.99f, 1.00f, 0.22f, 0.86f);
    style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_Header] = ImVec4(0.25f, 1.00f, 0.00f, 0.76f);
    style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 1.00f, 0.00f, 0.86f);
    style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_Column] = ImVec4(0.00f, 0.00f, 0.00f, 0.32f);
    style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.25f, 1.00f, 0.00f, 0.78f);
    style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.04f);
    style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.25f, 1.00f, 0.00f, 0.78f);
    style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
    style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}
