#include "modules.h"

#include <vector>

#include "imgui.h"
#include "mu/muParser.h"
#include "utils.h"

using namespace GraphAnalyze;

typedef struct
{
    float x, y, width, height;
    double minX, maxX, minY, maxY;
} GraphInfo;

GrapherModule::GrapherModule(int windowWidth, int windowHeight) : w(windowWidth), h(windowHeight)
{
    p.DefineVar("x", &x);
    p.SetExpr("x^2 + x + 1");
}

void GrapherModule::render()
{
    ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_FirstUseEver);
    if(!ImGui::Begin("Function graphing test", nullptr, ImGuiWindowFlags_NoResize))
    {
        ImGui::End();
        return;
    }
    
    if(invalidFunc)
        ImGui::PushStyleColor(ImGuiCol_FrameBg, 0xff0000ff);
    bool textModified = ImGui::InputText(" = f(x)", buf, MAX_FUNC_LENGTH);
    if(invalidFunc)
    {
        ImGui::PopStyleColor();
        if(textModified)
            invalidFunc = false;
    }
    
    if(ImGui::Button("Graph"))
    {
        try
        {
            p.SetExpr(std::string(buf));
            invalidFunc = false;
            readyToGraph = true;
            p.Eval();
            trace("Valid func");
        }
        catch(mu::Parser::exception_type &e)
        {
            invalidFunc = true;
            readyToGraph = false;
            trace("Invalid func");
        }
    }
    
    // if(readyToGraph)
    // {
    //     // Graph the function
    //     ImDrawList *drawList = ImGui::GetWindowDrawList();
    //     const float thickness = 2;
    //     const ImVec4 col = ImVec4(1, 1, 1, 1);
    //     const ImU32 col32 = ImColor(col);
    //     const ImVec2 pen = ImGui::GetCursorScreenPos();
    //     float x = pen.x, y = pen.y - 18;
    //
    //     for(unsigned int k = 0; k < xs.size() - 1; k++)
    //     {
    //         drawList->AddLine(ImVec2(x + scaleX(xs[k]), y + scaleY(ys[k])),
    //             ImVec2(x + scaleX(xs[k + 1]), y + scaleY(ys[k + 1])),
    //             col32, thickness);
    //     }
    // }
    ImGui::End();
}
