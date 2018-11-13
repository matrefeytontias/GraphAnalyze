#include "modules.h"

#include <algorithm>
#include <cmath>
#include <string>

#include "imgui.h"
#include "utils.h"
#include "GLFW/glfw3.h"

using namespace GraphAnalyze;

/**
 * Fills the class' variables with the numerically solved differential equation,
 * namely `xs`, `ys` and `gi`.
 * @param   boundaryX   X coordinate of the boundary conditions
 * @param   minX        low bound of the solving range
 * @param   maxX        high bound of the solving range
 * @param   dx          solving step
 */
void DiffEqSolverModule::solveDiffEq(double boundaryX, double minX, double maxX,
    double dx)
{
    std::vector<double> vs, nextvs;
    nextvs.resize(degree);
    
    // 1st order forward Euler
    // y' + fy = g <=> y_n+1 = (g_n - f_n * y_n) * dx + y_n
    
    // nth-degree diff eq : y'n + sum(k in 0 ... n-1, a_k * y'k) = b
    // Reduce nth-degree diff eq to n 1st-degree diff eqs
    // v_k := y'k for k in 0 ... n-1
    // n-1 diff eqs : v_k' = v_k+1 for k in 0 ... n-2
    // 1 more diff eq : v_n-1' + sum(k in 0 ... n-1, a_k * v_k) = b
    
    // Parallel 1st-order forward Euler
    // v_n-1_n+1 = (b_n - sum(k in 0 ... n-1, a_k_n * v_k_n)) * dx + v_n-1_n
    // for k in 0 ... n-2, v_k_n+1 = v_k+1_n * dx + v_k_n
    
    xs.clear();
    ys.clear();
    xs.push_back(boundaryX);
    ys.push_back(boundaryYs[0]);
    for(unsigned int k = 0; k < degree; k++)
        vs.push_back(boundaryYs[k]);
    
    // Go both ways from boundaryX
    // Do it this way for maximum array construction efficiency
    // boundaryX -> minX
    dx *= -1;
    for(x = boundaryX; x > minX; x += dx)
    {
        xs.push_back(x + dx);
        // Compute v_n-1_n+1 at the same time as all the other v_k_n
        nextvs[degree - 1] = bParser.Eval();
        for(unsigned int k = 0; k < degree - 1; k++)
        {
            nextvs[degree - 1] -= aParsers[k].Eval() * vs[k];
            nextvs[k] = vs[k + 1] * dx + vs[k];
        }
        // Extra iteration for k = n-1
        nextvs[degree - 1] -= aParsers[degree - 1].Eval() * vs[degree - 1];
        nextvs[degree - 1] *= dx;
        nextvs[degree - 1] += vs[degree - 1];
        // By construction, v_0 = y
        ys.push_back(nextvs[0]);
        
        for(unsigned int k = 0; k < degree; k++)
            vs[k] = nextvs[k];
    }
    std::reverse(xs.begin(), xs.end());
    std::reverse(ys.begin(), ys.end());
    
    // Restore the boundary values
    for(unsigned int k = 0; k < degree; k++)
        vs[k] = boundaryYs[k];
    
    // boundaryX -> maxX
    dx *= -1;
    for(x = boundaryX; x < maxX; x += dx)
    {
        xs.push_back(x + dx);
        nextvs[degree - 1] = bParser.Eval();
        for(unsigned int k = 0; k < degree - 1; k++)
        {
            nextvs[degree - 1] -= aParsers[k].Eval() * vs[k];
            nextvs[k] = vs[k + 1] * dx + vs[k];
        }
        nextvs[degree - 1] -= aParsers[degree - 1].Eval() * vs[degree - 1];
        nextvs[degree - 1] *= dx;
        nextvs[degree - 1] += vs[degree - 1];
        ys.push_back(nextvs[0]);
        for(unsigned int k = 0; k < degree; k++)
            vs[k] = nextvs[k];
    }
    
    gi.build(xs, ys);
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
    
    ImGui::Text("Equation degree : %d", degree); ImGui::SameLine();
    if(ImGui::Button("+##degreeUp"))
        degree += degree < MAX_DIFFEQ_DEGREE;
    ImGui::SameLine();
    if(ImGui::Button("-##degreeDown"))
        degree -= degree > 1;
    
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
    ImGui::PushTextWrapPos();
        ImGui::Text(eqText.c_str());
    ImGui::PopTextWrapPos();
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
        ImGui::PushItemWidth(windowW / 2 - nestedStartPos.x - ImGui::CalcTextSize("Boundary y'9(x) ").x);
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
        solveDiffEq(boundaryX, minX, maxX, dx);
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
