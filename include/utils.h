#ifndef INC_UTILS
#define INC_UTILS

#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>

#include "imgui.h"
#include "glad/glad.h"

/**
 * Makes an ImGui call with a colored background if a flag is set.
 * Uses GCC's non-standard statement expression extension.
 */
#define flashWidget(flag, color, call) \
({\
    bool f = flag;\
    if(f)\
        ImGui::PushStyleColor(ImGuiCol_FrameBg, color);\
    bool textModified = call;\
    if(f)\
        ImGui::PopStyleColor();\
    textModified;\
})

/**
 * Makes an ImGui call with a colored button color if a flag is set.
 * Uses GCC's non-standard statement expression extension.
 */
#define flashButtonWidget(flag, color, call) \
({\
    bool f = flag;\
    if(f)\
        ImGui::PushStyleColor(ImGuiCol_Button, color);\
    bool textModified = call;\
    if(f)\
        ImGui::PopStyleColor();\
    textModified;\
})

void setwd(char **argv);
std::string getFileContents(const char *path);
GLuint createShaderFromSource(GLenum type, const char *path);
void printShaderLog(GLuint shader);

#define checkGLerror() _checkGLerror(__FILE__, __LINE__)
void _checkGLerror(const char *file, int line);

#define trace(s) std::cerr << __FILE__ << ":" << __LINE__ << " : " << s << std::endl

#define fatal(s) do { std::stringstream ss; ss << __FILE__ << ":" << __LINE__ << " : " << s << std::endl; throw std::runtime_error(ss.str()); } while(0)

void displayTexture(GLint texture, float dx = 0.f, float dy = 0.f);

#endif
