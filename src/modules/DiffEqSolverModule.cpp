#include "modules.h"

#include <algorithm>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <vector>

using namespace GraphAnalyze;

static bool *openSolverModule;

DiffEqSolverModule::DiffEqSolverModule(bool *b)
{
    openSolverModule = b;
}

void DiffEqSolverModule::render(std::string name)
{
    if(!ImGui::Begin(name.c_str(), openSolverModule))
    {
        ImGui::End();
        return;
    }
    
    // ImVec2 windowSize = ImGui::GetWindowSize();
    // ImVec2 spacing = ImGui::GetStyle().ItemSpacing;
    // ImGui::SetCursorPosX((windowSize.x - ImGui::CalcTextSize("Solver").x)/2);
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        ImGui::Text("y'(x) + f(x)y(x) = g(x)");
    ImGui::PopFont();
    ImGui::Separator();
    static char fbuf[100] = { 0 };
    static char gbuf[100] = { 0 };
    ImGui::InputText("= f(x)", fbuf, 100);
    ImGui::InputText("= g(x)", gbuf, 50);
    ImGui::Separator();
    // ImGui::SetCursorPosX((windowSize.x - ImGui::CalcTextSize("Result").x) / 2);
    // ImGui::Text("Result");
    // ...


  ImGui::End();
}
