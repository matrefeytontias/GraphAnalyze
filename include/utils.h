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

/**
 * Sets the working directory.
 * @param   argv    expected  to be the second argument passed to `main`
 */
void setwd(char **argv);
/**
 * Returns a file's contents as an `std::string` given its path.
 */
std::string getFileContents(const char *path);
/**
 * Creates and compiles a shader from the path to its source.
 * @return  shader ID of the compiled shader
 */
GLuint createShaderFromSource(GLenum type, const char *path);
/**
 * Prints the log of a shader given its shader ID.
 */
void printShaderLog(GLuint shader);

/**
 * Prints all remaining OpenGL errors.
 */
#define checkGLerror() _checkGLerror(__FILE__, __LINE__)
void _checkGLerror(const char *file, int line);

/**
 * Writes to `std::cerr` with the stream notation, adding file name and line number.
 */
#define trace(s) std::cerr << __FILE__ << ":" << __LINE__ << " : " << s << std::endl

/**
 * Throws a runtime error exception with a string built with the stream notation.
 */
#define fatal(s) do { std::stringstream ss; ss << __FILE__ << ":" << __LINE__ << " : " << s << std::endl; throw std::runtime_error(ss.str()); } while(0)

/**
 * Displays a texture in a rectangle half the width and height of the window, with offsets.
 */
void displayTexture(GLint texture, float dx = 0.f, float dy = 0.f);

#endif
