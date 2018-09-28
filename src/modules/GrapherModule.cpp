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
    void build(const std::vector<double> &xs, std::vector<double> &ys)
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

GrapherModule::GrapherModule(int windowWidth, int windowHeight) : w(windowWidth), h(windowHeight)
{
    p.DefineVar("x", &x);
    p.SetExpr("x^2 + x + 1");
    
    for(int k = 0; k <= 1000; k++)
        xs.push_back((k - 500.) / 500.);
}

/*
bool ImGuiInputText(const char *label, char *buf, size_t bufSize, bool invalid = false)
{
    if(invalid)
        ImGui::PushStyleColor(ImGuiCol_FrameBg, 0xff0000ff);
    bool textModified = ImGui::InputText(label, buf, bufSize);
    if(invalid)
        ImGui::PopStyleColor();
    return textModified;
}
*/

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

void GrapherModule::render()
{
    static GraphInfo gi;
    
    ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_FirstUseEver);
    if(!ImGui::Begin("Function graphing test", nullptr))
    {
        ImGui::End();
        return;
    }
    
    ImGui::TextUnformatted("Domain definition");
    static float minX = -1., maxX = 1.;
    bool valueChanged = false;
    float w = ImGui::GetWindowWidth();
    ImGui::PushItemWidth(w / 2 - 32);
        valueChanged |= invalidate(minX == maxX, ImGui::DragFloat("Min X", &minX, 0.1, -1e12, maxX)); ImGui::SameLine();
    ImGui::PopItemWidth();
    ImGui::PushItemWidth(-32);
        valueChanged |= invalidate(minX == maxX, ImGui::DragFloat("Max X", &maxX, 0.1, minX, 1e12));
    ImGui::PopItemWidth();
    
    if(invalidate(invalidFunc, ImGui::InputText(" = f(x)", buf, MAX_FUNC_LENGTH, invalidFunc)))
        invalidFunc = false;
    
    if(ImGui::Button("Graph"))
    {
        if(minX != maxX)
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
    }
    
    ImGui::Separator();
    
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
}
