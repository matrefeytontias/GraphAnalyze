#include "modules.h"

#include <cmath>

#include "imgui.h"
#include "utils.h"
#include "GLFW/glfw3.h"

using namespace GraphAnalyze;

void DiffEqSolverModule::render()
{
    static bool valueChanged = false;
    
    if(!*open)
        return;
    ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_FirstUseEver);
    if(!ImGui::Begin("Differential equations solver", open))
    {
        ImGui::End();
        return;
    }
    
    const int windowW = ImGui::GetWindowWidth(),
        windowH = ImGui::GetWindowHeight();
    (void)windowH;
    ImVec2 nestedStartPos;
    
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        ImGui::Text("y'(x) + f(x)y(x) = g(x)");
    ImGui::PopFont();
    ImGui::Separator();
    
    static char fbuf[MAX_FUNC_LENGTH] = { 0 },
        gbuf[MAX_FUNC_LENGTH] = { 0 };
    
    if(ImGui::TreeNode("Functions definition"))
    {
        nestedStartPos = ImGui::GetCursorPos();
        ImGui::PushItemWidth(windowW / 2 - nestedStartPos.x - ImGui::CalcTextSize("=: f(x)").x);
            valueChanged |= ImGui::InputText("=: f(x)", fbuf, MAX_FUNC_LENGTH); ImGui::SameLine();
            valueChanged |= ImGui::InputText("=: g(x)", gbuf, MAX_FUNC_LENGTH);
        ImGui::PopItemWidth();
        ImGui::TreePop();
    }
    
    static double boundaryX = 0, boundaryY = 0;
    static float minX = 0, maxX = 1;
    static double dt = 0.001;
    
    if(ImGui::TreeNode("Domain and boundaries"))
    {
        // Needed in case the previous tree nodes are collapsed
        nestedStartPos = ImGui::GetCursorPos();
        ImGui::PushItemWidth(windowW / 2 - nestedStartPos.x - ImGui::CalcTextSize("Boundary X").x);
            valueChanged |= flashWidget(boundaryX < minX || boundaryX > maxX, 0xff0000ff,
                ImGui::InputDouble("Boundary X", &boundaryX)); ImGui::SameLine();
            float startx = ImGui::GetCursorPosX();
            valueChanged |= ImGui::InputDouble("Boundary Y", &boundaryY);
            valueChanged |= flashWidget(minX >= maxX, 0xff0000ff,
                ImGui::DragFloat("Min X", &minX, 0.1f, -FLT_MAX, maxX));
            ImGui::SameLine();
            ImGui::SetCursorPosX(startx);
            valueChanged |= flashWidget(minX >= maxX, 0xff0000ff,
                ImGui::DragFloat("Max X", &maxX, 0.1f, minX, FLT_MAX));
        ImGui::PopItemWidth();
        ImGui::TreePop();
    }
    
    if(ImGui::TreeNode("Solving parameters"))
    {
        valueChanged |= flashWidget(dt <= 0, 0xff0000ff, ImGui::InputDouble("Precision", &dt));
        ImGui::TreePop();
    }
    
    // Always draw the button, even if the domain is wrong
    ImU32 graphButtonColor = clerp(ImGui::GetStyle().Colors[ImGuiCol_Button],
        ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered],
        sin(glfwGetTime() * M_PI * 2) / 2. + 0.5);
    if(flashButtonWidget(valueChanged, graphButtonColor, ImGui::Button("Graph")) && minX < maxX)
    {
        // do stuff
    }
    
    ImGui::End();
}
