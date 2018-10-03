#include "modules.h"

#include <algorithm>
#include <iomanip>
#include <numeric>
#include <sstream>
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
    double minX = -1., maxX = 1., minY, maxY;
    bool ready = false;
    /**
     * Sets the drawing area. If width and height aren't provided, use all of the
     * available space.
     */
    void updateArea(int w = -1, int h = -1)
    {
        pos = ImGui::GetCursorScreenPos();
        size = w < 0 || h < 0 ? ImGui::GetContentRegionAvail() : ImVec2(w, h);
    }
    
    /**
     * Builds the graph info from arrays of X and Y.
     */
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
    
    /**
     * Maps an (x, y) value in graph space to function space.
     */
    inline ImVec2 unscale(float x, float y)
    {
        return ImVec2((x - pos.x) * (maxX - minX) / size.x + minX,
            (y - pos.y) * (maxY - minY) / size.y + minY);
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

/**
 * Builds the arrays of X and Y from a minX and a maxX.
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

std::string toString(double v, int precision = 0)
{
    std::ostringstream sstream;
    if(precision > 0)
        sstream << std::setprecision(precision);
    sstream << v;
    return sstream.str();
}

/**
 * Draws the built function using the current graph info.
 */
void GrapherModule::plotFunction(int w, int h)
{
    gi.updateArea(w, h);
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    ImVec2 top = gi.pos, bot(gi.pos.x + gi.size.x + 1, gi.pos.y + gi.size.y + 1),
    origin = gi.scale(0, 0);
    
    drawList->AddRectFilled(top, bot, 0xffffffff);
    
    ImGui::PushClipRect(top, bot, true);
        double xrange = gi.maxX - gi.minX,
            yrange = gi.maxY - gi.minY;
        std::string s = toString((gi.minX + gi.maxX) / 2);
        
        // Draw the axis system
        int xTickSpace = (int)ImGui::CalcTextSize("0.00000").x,
            yTickSpace = (int)ImGui::CalcTextSize("0.000").x;
        ImU32 col32 = 0xff888888;
        drawList->AddLine(ImVec2(origin.x, top.y), ImVec2(origin.x, top.y + h), col32, 1);
        drawList->AddLine(ImVec2(top.x, origin.y), ImVec2(top.x + w, origin.y), col32, 1);
        
        // Draw the X axis' ticks
        int nbTicks = (w / xTickSpace) & ~1;
        for(int k = 0; k <= nbTicks; k++)
        {
            int x = top.x + w * k / nbTicks;
            // Avoid tick overlap
            if(abs(x - origin.x) >= xTickSpace)
            {
                drawList->AddLine(ImVec2(x, origin.y - 5), ImVec2(x, origin.y + 5), col32, 1);
                std::string s = toString(gi.minX + xrange * k / nbTicks, 2);
                ImVec2 textSize = ImGui::CalcTextSize(s.c_str());
                drawList->AddText(ImVec2(x - textSize.x / 2, std::min(bot.y - textSize.y, origin.y + 5)),
                    col32, s.c_str());
            }
        }
        
        // Draw the Y axis' ticks
        if(yrange > 0)
        {
            nbTicks = (h / yTickSpace) & ~1;
            for(int k = 0; k <= nbTicks; k++)
            {
                int y = bot.y - h * k / nbTicks;
                // Avoid tick overlap
                if(abs(y - origin.y) >= yTickSpace)
                {
                    drawList->AddLine(ImVec2(origin.x - 5, y), ImVec2(origin.x + 5, y), col32, 1);
                    std::string s = toString(gi.minY + yrange * k / nbTicks, 2);
                    drawList->AddText(ImVec2(origin.x + 2, y), col32, s.c_str());
                }
            }
        }
        
        // Plot the actual function
        col32 = 0xff000000;
        for(unsigned int k = 0; k + 1 < ys.size(); k++)
        {
            drawList->AddLine(gi.scale(xs[k], ys[k]), gi.scale(xs[k + 1], ys[k + 1]),
                col32, 1);
        }
    ImGui::PopClipRect();
}

/**
 * Renders the module.
 */
void GrapherModule::render()
{
    const ImVec2 buttonSize = ImVec2(60, 30);
    static float minX = -1.f, maxX = 1.f;
    
    ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_FirstUseEver);
    if(!ImGui::Begin("Function graphing test", nullptr))
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
        if(displayTangents)
        {
            trace("Display the tangents pls");
        }
        if(ImGui::Button("op2", buttonSize))
        {
            //todo
        }

        if(ImGui::Button("op3", buttonSize))
        {
            //todo
        }
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
        int startPosGraph = ImGui::GetCursorPosX();
        
        ImGui::PushItemWidth(windowW - startPosGraph - hSpacing * 2 - ImGui::CalcTextSize(" = f(x)").x);
            if(invalidate(invalidFunc, ImGui::InputText(" = f(x)", buf, MAX_FUNC_LENGTH)))
                invalidFunc = false;
        ImGui::PopItemWidth();
        ImGui::PushItemWidth((windowW - startPosGraph - 20) / 3);
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
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
            int bottomY = windowH - buttonSize.y - vSpacing * 2;
            if(gi.ready)
                plotFunction(windowW - startPosGraph - hSpacing,
                    bottomY - ImGui::GetCursorPosY() - vSpacing);
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
