#include "modules.h"

#include <algorithm>
#include <numeric>
#include <vector>

#include "imgui.h"
#include "mu/muParser.h"
#include "utils.h"

/**
 * Makes an ImGui call with a red background if a flag is set.
 * Uses GCC's non-standard statement expression extension.
 */
#define invalidate(flag, call) \
({\
    bool f = flag;\
    if(f)\
        ImGui::PushStyleColor(ImGuiCol_FrameBg, 0xff0000ff);\
    bool textModified = call;\
    if(f)\
        ImGui::PopStyleColor();\
    textModified;\
})

using namespace GraphAnalyze;

typedef struct
{
    ImVec2 pos, size;
    double minX, maxX, minY, maxY;
    bool ready = false;
    void updateArea()
    {
        pos = ImGui::GetCursorScreenPos();
        size = ImGui::GetContentRegionAvail();
    }
    void build(const std::vector<float> &xs, std::vector<float> &ys)
    {
        static auto minComputer = [](double a, double b) { return std::min(a, b); };
        static auto maxComputer = [](double a, double b) { return std::max(a, b); };
        updateArea();
        minX = std::accumulate(xs.begin(), xs.end(), xs[0], minComputer);
        maxX = std::accumulate(xs.begin(), xs.end(), xs[0], maxComputer);
        minY = std::accumulate(ys.begin(), ys.end(), ys[0], minComputer);
        maxY = std::accumulate(ys.begin(), ys.end(), ys[0], maxComputer);
        ready = true;
    }
    /**
     * Maps an (x, y) value in function space to graph space.
     */
    inline ImVec2 scale(double x, double y)
    {
        return ImVec2((x - minX) * size.x / (maxX - minX) + pos.x,
            size.y - (y - minY) * size.y / (maxY - minY) + pos.y);
    }
} GraphInfo;

static GraphInfo gi;

GrapherModule::GrapherModule(int windowWidth, int windowHeight) : w(windowWidth), h(windowHeight)
{
    p.DefineVar("x", &x);
    p.SetExpr("x^2 + x + 1");
    
    for(int k = 0; k <= 1000; k++)
        xs.push_back((k - 500.) / 500.);
}

void GrapherModule::evaluateFunction(double minX, double maxX)
{
    xs.clear();
    ys.clear();
    for(unsigned int k = 0; k < 1000; k++)
    {
        xs.push_back(x = (maxX - minX) * k / 999 + minX);
        ys.push_back(p.Eval());
    }
}

void GrapherModule::plotFunction(int w, int h)
{
    ImGui::PlotLines("", &ys[0], ys.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(w, h), sizeof(float));
}

void GrapherModule::render()
{
    ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_FirstUseEver);
    if(!ImGui::Begin("Function graphing test", nullptr))
    {
        ImGui::End();
        return;
    }
    
    float startPos = (ImGui::GetWindowSize().x - ImGui::CalcTextSize("General Tools").x) / 2;
    ImGui::SetCursorPosX(startPos);
    
    ImGui::Text("General Tools");
    
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - (50 * 4 + ImGui::GetStyle().ItemSpacing.x * 3)) / 2);
    if(ImGui::Button("Open",ImVec2(50,30)))
    {
        //todo
    }
    ImGui::SameLine();
    if(ImGui::Button("Close",ImVec2(50,30)))
    {
        //todo
    }
    ImGui::SameLine();
    if(ImGui::Button("Undo",ImVec2(50,30)))
    {
        //todo
    }
    ImGui::SameLine();
    if(ImGui::Button("Redo",ImVec2(50,30)))
    {
        //todo
    }
    ImGui::Separator();
    ImGui::NewLine();
    ImGui::BeginGroup();
        ImGui::Text("Mathematical Tools");
        ImGui::NewLine();
        if(ImGui::Button("op1",ImVec2(50,30)))
        {
            //todo
        }
        if(ImGui::Button("op2",ImVec2(50,30)))
        {
            //todo
        }

        if(ImGui::Button("op3",ImVec2(50,30)))
        {
            //todo
        }
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
        int startPosGraph = ImGui::GetCursorPosX();
        static float minX = 0.0f, maxX = 10.0f;
        ImGui::PushItemWidth(ImGui::GetWindowWidth() - startPosGraph - 20 - 100);
            if(invalidate(invalidFunc, ImGui::InputText(" = f(x)", buf, MAX_FUNC_LENGTH)))
                invalidFunc = false;
        ImGui::PopItemWidth();
        ImGui::PushItemWidth((ImGui::GetWindowWidth() - startPosGraph - 20) / 3);
            invalidate(minX == maxX, ImGui::DragFloat("Min X", &minX, 0.1f, -FLT_MAX, maxX));
            ImGui::SameLine();
            invalidate(minX == maxX, ImGui::DragFloat("Max X", &maxX, 0.1f, minX, FLT_MAX));
            ImGui::SameLine();
            if(minX != maxX && ImGui::Button("Graph"))
            {
                try
                {
                    p.SetExpr(std::string(buf));
                    invalidFunc = false;
                    evaluateFunction(minX, maxX);
                    gi.build(xs, ys);
                    trace("Valid func");
                }
                catch(mu::Parser::exception_type &e)
                {
                    invalidFunc = true;
                    gi.ready  = false;
                    trace("Invalid func");
                }
            }
        ImGui::PopItemWidth();
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
            plotFunction(ImGui::GetWindowWidth() - startPosGraph - 60,
                ImGui::GetWindowHeight() - ImGui::GetCursorPosY() - 100);
            float tabSize = (ImGui::GetWindowWidth() - startPosGraph - 20) / 2 - 50;
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
                if(ImGui::Button("+", ImVec2(50, 30)))
                {
                    //todo
                }
            ImGui::PopStyleVar();
        ImGui::PopStyleVar();
    ImGui::EndGroup();
    ImGui::End();
    
    // Custom rendering code for later possibly
    /*
    if(gi.ready)
    {
        gi.updateArea();
        // Graph the function
        ImDrawList *drawList = ImGui::GetWindowDrawList();
        const float thickness = 2;
        const ImU32 col32 = 0xffffffff;
    
        for(unsigned int k = 0; k < xs.size() - 1; k++)
        {
            drawList->AddLine(gi.scale(xs[k], ys[k]), gi.scale(xs[k + 1], ys[k + 1]),
                col32, thickness);
        }
    }
    ImGui::End();
    */
}
