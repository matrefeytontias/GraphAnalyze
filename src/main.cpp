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
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();
    
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    
    trace("Entering drawing loop");
    
    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplGlfwGL3_NewFrame();
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
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
