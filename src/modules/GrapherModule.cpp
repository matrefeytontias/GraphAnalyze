#include "modules.h"

#include <iomanip>
#include <sstream>
#include <vector>

#include "imgui.h"
#include "utils.h"
#include "GLFW/glfw3.h"
#include "mu/muParser.h"

using namespace GraphAnalyze;

GrapherModule::GrapherModule(bool *open, int windowWidth, int windowHeight) : open(open), w(windowWidth), h(windowHeight), ism(this)
{
    p.DefineVar("x", &x);
    p.SetExpr("x^2 + x + 1");
    
    for(int k = 0; k <= PLOT_INTERVALS; k++)
        xs.push_back(2. * k / PLOT_INTERVALS - 1.);
}

/**
 * Call whenever minX, maxX or the function expression changes.
 */
void GrapherModule::refreshFunctionData()
{
    evaluateFunction();
    gi.build(xs, ys);
}

/**
 * Builds the arrays of X and Y from this.minX and this.maxX.
 */
void GrapherModule::evaluateFunction()
{
    xs.clear();
    ys.clear();
    for(unsigned int k = 0; k <= PLOT_INTERVALS; k++)
    {
        xs.push_back(x = (maxX - minX) * k / PLOT_INTERVALS + minX);
        ys.push_back(p.Eval());
    }
}

/**
 * Handles the user clicking and dragging to zoom on a specific X range.
 * /!\ This needs the graph widget to be the last drawn widget.
 */
void GrapherModule::handleZoom()
{
    static float x1 = minX, x2 = maxX;
    if(GraphAnalyze::userSelectArea(gi, &x1, &x2, false, true) && x1 != x2)
    {
        minX = std::min(x1, x2);
        maxX = std::max(x1, x2);
        refreshFunctionData();
    }
}

/**
 * Plots the tangent to the function at the position of the mouse.
 * /!\ This needs the graph widget to be the last drawn widget.
 */
void GrapherModule::plotTangent(float length)
{
    if(ImGui::IsItemHovered())
    {
        ImDrawList *drawList = ImGui::GetWindowDrawList();
        int mouseX = ImGui::GetMousePos().x;
        int index = (int)((mouseX - gi.pos.x) * PLOT_INTERVALS / gi.size.x);
        ImVec2 p = gi.scale(xs[index], ys[index]),
            np = gi.scale(xs[index + 1], ys[index + 1]);
        ImVec2 df(np.x - p.x, np.y - p.y);
        float l = sqrt(df.x * df.x + df.y * df.y);
        df.x /= l; df.y /= l;
        
        ImVec2 origin = gi.scale(xs[index], ys[index]);
        
        drawList->AddLine(ImVec2(origin.x, gi.pos.y), ImVec2(origin.x, gi.pos.y + gi.size.y),
            0xff0000ff, 1);
        drawList->AddLine(ImVec2(origin.x - df.x * length, origin.y - df.y * length),
            ImVec2(origin.x + df.x * length, origin.y + df.y * length), 0xff0000ff,
            4);
        
        // Add orthonormal view of the tangent
        df = ImVec2(xs[index + 1] - xs[index], ys[index + 1] - ys[index]);
        l = sqrt(df.x * df.x + df.y * df.y);
        df.x /= l; df.y /= l;
        
        ImVec2 textSize = ImGui::CalcTextSize("Orthonormal");
        float orthoLen = (textSize.x - textSize.y) / 2;
        
        ImVec2 boxBase(gi.pos.x + gi.size.x - textSize.x - 3, 0);
        boxBase.y = origin.x + length > boxBase.x && origin.y - length < gi.pos.y + textSize.x
            ? gi.pos.y + gi.size.y - textSize.x - 1 : gi.pos.y - 1;
        drawList->AddRectFilled(ImVec2(boxBase.x, boxBase.y),
            ImVec2(boxBase.x + textSize.x + 3, boxBase.y + textSize.x + 2),
            0xff000000);
        drawList->AddRectFilled(ImVec2(boxBase.x + 1, boxBase.y + 1),
            ImVec2(boxBase.x + textSize.x + 2, boxBase.y + textSize.x + 1),
            0xffffffff);
        
        drawList->AddText(ImVec2(boxBase.x + 1, boxBase.y + 1), 0xff000000, "Orthonormal");
        
        ImVec2 orthoBase(boxBase.x + textSize.x / 2, boxBase.y + textSize.y + orthoLen);
        drawList->AddLine(ImVec2(orthoBase.x - df.x * orthoLen, orthoBase.y + df.y * orthoLen),
            ImVec2(orthoBase.x + df.x * orthoLen, orthoBase.y - df.y * orthoLen), 0xff0000ff, 4);
    }
}

/**
 * Renders the module.
 */
void GrapherModule::render()
{
    const ImVec2 buttonSize = ImVec2(60, 30);
    
    ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_FirstUseEver);
    if(!*open)
        return;
    if(!ImGui::Begin("Function graphing", open))
    {
        ImGui::End();
        return;
    }
    
    const int hSpacing = ImGui::GetStyle().ItemSpacing.x,
        vSpacing = ImGui::GetStyle().ItemSpacing.y,
        windowW = ImGui::GetWindowWidth(),
        windowH = ImGui::GetWindowHeight();
    
    float startPos = (windowW - ImGui::CalcTextSize("General Tools").x) / 2;
    ImGui::SetCursorPosX(startPos);
    
    ImGui::Text("General Tools");
    
    ImGui::SetCursorPosX((windowW - (buttonSize.x * 4 + hSpacing * 3)) / 2);
    if(ImGui::Button("Open", buttonSize))
    {
        //todo
    }
    ImGui::SameLine();
    if(ImGui::Button("Close", buttonSize))
    {
        //todo
    }
    ImGui::SameLine();
    if(ImGui::Button("Undo", buttonSize))
    {
        //todo
    }
    ImGui::SameLine();
    if(ImGui::Button("Redo", buttonSize))
    {
        //todo
    }
    ImGui::Separator();
    ImGui::NewLine();
    ImGui::BeginGroup();
        ImGui::Text("Mathematical Tools");
        ImGui::NewLine();
        static bool displayTangents = false;
        ImGui::Checkbox("Tangents", &displayTangents);
        if(ImGui::Button("Integrate", buttonSize))
            ism.active = true;
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
        int startPosGraph = ImGui::GetCursorPosX();
        static bool valueChanged = false;
        
        ImGui::PushItemWidth(windowW - startPosGraph - hSpacing * 2 - ImGui::CalcTextSize(" =: f(x)").x);
        valueChanged |= flashWidget(invalidFunc, 0xff0000ff,
            GraphAnalyze::InputFunction(" =: f(x)", buf, MAX_FUNC_LENGTH, p, &invalidFunc));
        ImGui::PopItemWidth();
        ImGui::PushItemWidth((windowW - startPosGraph - 20) / 3);
            valueChanged |= flashWidget(minX >= maxX, 0xff0000ff, ImGui::DragFloat("Min X", &minX, 0.1f, -FLT_MAX, maxX));
            ImGui::SameLine();
            valueChanged |= flashWidget(minX >= maxX, 0xff0000ff, ImGui::DragFloat("Max X", &maxX, 0.1f, minX, FLT_MAX));
            ImGui::SameLine();
            // Always draw the button, even if the domain is wrong
            ImU32 graphButtonColor = clerp(ImGui::GetStyle().Colors[ImGuiCol_Button],
                ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered],
                sin(glfwGetTime() * M_PI * 2) / 2. + 0.5);
            if(flashButtonWidget(valueChanged, graphButtonColor, ImGui::Button("Graph"))
                && !invalidFunc && minX < maxX)
            {
                valueChanged = false;
                refreshFunctionData();
            }
        ImGui::PopItemWidth();
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
            int bottomY = windowH - buttonSize.y - vSpacing * 2;
            ImVec2 plotSize(windowW - startPosGraph - hSpacing,
                bottomY - ImGui::GetCursorPosY() - vSpacing);
            if(gi.ready)
            {
                ImGui::PushClipRect(gi.pos, ImVec2(gi.pos.x + gi.size.x, gi.pos.y + gi.size.y), true);
                    GraphAnalyze::GraphWidget(gi, xs, ys, plotSize.x, plotSize.y);
                    if(hasClick)
                        handleZoom();
                    if(displayTangents)
                        plotTangent();
                    ism.render();
                ImGui::PopClipRect();
            }
            ImGui::SetCursorPosY(bottomY);
            float tabSize = (windowW - startPosGraph - 20) / 2 - 50;
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, .1f);
                if(ImGui::Button("Tab1", ImVec2(tabSize, 30)))
                {
                    //todo
                }
                ImGui::SameLine();
                if(ImGui::Button("Tab2", ImVec2(tabSize, 30)))
                {
                    //todo
                }
                ImGui::SameLine();
                if(ImGui::Button("+", buttonSize))
                {
                    //todo
                }
            ImGui::PopStyleVar();
        ImGui::PopStyleVar();
    ImGui::EndGroup();
    ImGui::End();
}
