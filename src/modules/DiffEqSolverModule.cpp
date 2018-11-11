#include "modules.h"

#include <algorithm>
#include <cmath>

#include "imgui.h"
#include "utils.h"
#include "GLFW/glfw3.h"

using namespace GraphAnalyze;

/**
 * Fills the class' variables with the numerically solved diff eq.
 */
void DiffEqSolverModule::solveDiffEq(double boundaryX, double boundaryY, double minX, double maxX, double dx)
{
    // 1st order forward Euler
    // y' + fy = g <=> y_n+1 = (g(x_n) - f(x_n)y_n) * dx + y_n
    xs.clear();
    ys.clear();
    xs.push_back(boundaryX);
    ys.push_back(boundaryY);
    // Go both ways from boundaryX
    // Do it this way for maximum array construction efficiency
    dx *= -1;
    for(x = boundaryX; x > minX; x += dx)
    {
        float y_n = ys.back();
        xs.push_back(x + dx);
        ys.push_back((gparser.Eval() - fparser.Eval() * y_n) * dx + y_n);
    }
    std::reverse(xs.begin(), xs.end());
    std::reverse(ys.begin(), ys.end());
    
    dx *= -1;
    for(x = boundaryX; x < maxX; x += dx)
    {
        float y_n = ys.back();
        xs.push_back(x + dx);
        ys.push_back((gparser.Eval() - fparser.Eval() * y_n) * dx + y_n);
    }
    
    gi.build(xs, ys);
}

void DiffEqSolverModule::render()
{
    static bool valueChanged = false, invalidF = false, invalidG = false;
    
    if(!*open)
        return;
    ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_FirstUseEver);
    if(!ImGui::Begin("Differential equations solver", open))
    {
        ImGui::End();
        return;
    }
    
    const float windowW = ImGui::GetWindowWidth(),
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
            valueChanged |= flashWidget(invalidF, 0xff0000ff,
                GraphAnalyze::InputFunction(" =: f(x)", fbuf, MAX_FUNC_LENGTH, fparser, &invalidF));
            valueChanged |= flashWidget(invalidG, 0xff0000ff,
                GraphAnalyze::InputFunction(" =: g(x)", gbuf, MAX_FUNC_LENGTH, gparser, &invalidG));
        ImGui::PopItemWidth();
        ImGui::TreePop();
    }
    
    static double boundaryX = 0, boundaryY = 0;
    static float minX = 0, maxX = 1;
    static double dx = 0.001;
    
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
        valueChanged |= flashWidget(dx <= 0, 0xff0000ff, ImGui::InputDouble("Precision", &dx));
        ImGui::TreePop();
    }
    
    // Always draw the button, even if the domain is wrong
    ImU32 graphButtonColor = clerp(ImGui::GetStyle().Colors[ImGuiCol_Button],
        ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered],
        sin(glfwGetTime() * M_PI * 2) / 2. + 0.5);
    if(flashButtonWidget(valueChanged, graphButtonColor, ImGui::Button("Solve")) && minX < maxX
        && !invalidF && !invalidG)
    {
        solveDiffEq(boundaryX, boundaryY, minX, maxX, dx);
        valueChanged = false;
    }
    
    if(gi.ready)
    {
        const ImVec2 pos = ImGui::GetCursorPos();
        // Graph the calculated solution
        GraphAnalyze::GraphWidget(gi, xs, ys, windowW - pos.x * 2, windowH - pos.y - pos.x);
    }
    
    ImGui::End();
}
