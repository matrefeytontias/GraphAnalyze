#include "modules.h"

#include "imgui.h"

#include "utils.h"

using namespace GraphAnalyze;

void DiffEqSolverModule::render()
{
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
    const ImVec2 startPos = ImGui::GetCursorPos();
    
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        ImGui::Text("y'(x) + f(x)y(x) = g(x)");
    ImGui::PopFont();
    ImGui::Separator();
    
    static char fbuf[MAX_FUNC_LENGTH] = { 0 },
        gbuf[MAX_FUNC_LENGTH] = { 0 };
    
    ImGui::PushItemWidth(windowW / 2 - startPos.x * 2 - ImGui::CalcTextSize("=: f(x)").x);
        ImGui::InputText("=: f(x)", fbuf, MAX_FUNC_LENGTH); ImGui::SameLine();
        ImGui::InputText("=: g(x)", gbuf, MAX_FUNC_LENGTH);
    ImGui::PopItemWidth();
    ImGui::Separator();
    
    static double boundaryX = 0, boundaryY = 0;
    static float minX = 0, maxX = 1;
    
    ImGui::Text("Domain and boundary definition");
    ImGui::BeginGroup();
    ImGui::PushItemWidth(windowW / 2 - startPos.x * 2 - ImGui::CalcTextSize("Boundary X").x);
        flashWidget(boundaryX < minX || boundaryX > maxX, 0xff0000ff,
            ImGui::InputDouble("Boundary X", &boundaryX)); ImGui::SameLine();
        float startx = ImGui::GetCursorPosX();
        ImGui::InputDouble("Boundary Y", &boundaryY);
        flashWidget(minX >= maxX, 0xff0000ff, ImGui::DragFloat("Min X", &minX, 0.1f, -FLT_MAX, maxX));
        ImGui::SameLine();
        ImGui::SetCursorPosX(startx);
        flashWidget(minX >= maxX, 0xff0000ff, ImGui::DragFloat("Max X", &maxX, 0.1f, minX, FLT_MAX));
        ImGui::SameLine();
    ImGui::PopItemWidth();
    ImGui::EndGroup();
    ImGui::Separator();
    
    ImGui::Button("Solve");
    
    ImGui::End();
}
