#include "modules.h"

#include <algorithm>
#include <cmath>
#include <string>

#include "imgui.h"
#include "utils.h"
#include "GLFW/glfw3.h"

using namespace GraphAnalyze;

/**
 * Fills the class' variables with the numerically solved diff eq.
 */
void DiffEqSolverModule::solveDiffEq(double boundaryX, double minX, double maxX,
    double dx)
{
    (void)boundaryX;
    (void)minX;
    (void)maxX;
    (void)dx;
    /*
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
    */
}

void DiffEqSolverModule::render()
{
    static bool valueChanged = false;
    
    if(!*open)
        return;
    ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_Once);
    if(!ImGui::Begin("Differential equations solver", open))
    {
        ImGui::End();
        return;
    }
    
    const float windowW = ImGui::GetWindowWidth(),
        windowH = ImGui::GetWindowHeight();
    ImVec2 nestedStartPos;
    
    auto derivOfDegree = [](int n)
    {
        std::string text;
        switch(n)
        {
        case 0:
            text = "y(x)";
            break;
        case 1:
            text = "y'(x)";
            break;
        case 2:
            text = "y''(x)";
            break;
        case 3:
            text = "y'''(x)";
            break;
        default:
            text = "y'" + std::to_string(n) + "(x)";
        }
        return text;
    };
    
    std::string eqText = derivOfDegree(degree);
    
    for(int k = degree - 1; k >= 0; k--)
        eqText += " + a" + std::to_string(k) + "(x)" + derivOfDegree(k);
    
    eqText += " = b(x)";
    
    if(ImGui::Button("+##degreeUp"))
        degree += degree < MAX_DIFFEQ_DEGREE;
    ImGui::SameLine();
    if(ImGui::Button("-##degreeDown"))
        degree -= degree > 1;
    
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        ImGui::Text(eqText.c_str());
    ImGui::PopFont();
    ImGui::Separator();
    
    bool anyInvalid = false;
    
    if(ImGui::TreeNode("Functions definition"))
    {
        nestedStartPos = ImGui::GetCursorPos();
        for(int k = degree - 1; k >= 0; k--)
        {
            std::string funcName = " =: a" + std::to_string(k) + "(x)";
            valueChanged |= flashWidget(invalids[k], 0xff0000ff,
                GraphAnalyze::InputFunction(funcName.c_str(), aBufs[k], MAX_FUNC_LENGTH, aParsers[k], &invalids[k]));
            anyInvalid |= invalids[k];
        }
        
        valueChanged |= flashWidget(bInvalid, 0xff0000ff,
            GraphAnalyze::InputFunction(" =: b(x)", bBuf, MAX_FUNC_LENGTH, bParser, &bInvalid));
        anyInvalid |= bInvalid;
        
        ImGui::TreePop();
    }
    
    static double boundaryX = 0;
    static float minX = 0, maxX = 1;
    static double dx = 0.001;
    
    if(ImGui::TreeNode("Domain and boundaries"))
    {
        // Needed in case the previous tree nodes are collapsed
        nestedStartPos = ImGui::GetCursorPos();
        ImGui::PushItemWidth(windowW / 2 - nestedStartPos.x - ImGui::CalcTextSize("Boundary y'9(x)").x);
            valueChanged |= flashWidget(minX >= maxX, 0xff0000ff,
                ImGui::DragFloat("Min X", &minX, 0.1f, -FLT_MAX, maxX));
            ImGui::SameLine();
            valueChanged |= flashWidget(minX >= maxX, 0xff0000ff,
                ImGui::DragFloat("Max X", &maxX, 0.1f, minX, FLT_MAX));
            valueChanged |= flashWidget(boundaryX < minX || boundaryX > maxX, 0xff0000ff,
                ImGui::InputDouble("Boundary X", &boundaryX));
            
            for(int k = degree - 1; k >= 0; k--)
            {
                std::string funcName = "Boundary " + derivOfDegree(k);
                valueChanged |= ImGui::InputDouble(funcName.c_str(), &boundaryYs[k]);
                if((degree - k) % 2 && k != 0)
                    ImGui::SameLine();
            }
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
        && !anyInvalid)
    {
        // solveDiffEq(boundaryX, boundaryY, minX, maxX, dx);
        // valueChanged = false;
    }
    
    if(gi.ready)
    {
        const ImVec2 pos = ImGui::GetCursorPos();
        // Graph the calculated solution
        GraphAnalyze::GraphWidget(gi, xs, ys, windowW - pos.x * 2, windowH - pos.y - pos.x);
    }
    
    ImGui::End();
}
