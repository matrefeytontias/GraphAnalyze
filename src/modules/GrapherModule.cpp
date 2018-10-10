#include "modules.h"

#include <iomanip>
#include <sstream>
#include <vector>

#include "imgui.h"
#include "utils.h"
#include "GLFW/glfw3.h"
#include "mu/muParser.h"

/**
 * Makes an ImGui call with a colored background if a flag is set.
 * Uses GCC's non-standard statement expression extension.
 */
#define flashWidget(flag, color, call) \
({\
    bool f = flag;\
    if(f)\
        ImGui::PushStyleColor(ImGuiCol_FrameBg, color);\
    bool textModified = call;\
    if(f)\
        ImGui::PopStyleColor();\
    textModified;\
})

/**
 * Makes an ImGui call with a colored button color if a flag is set.
 * Uses GCC's non-standard statement expression extension.
 */
#define flashButtonWidget(flag, color, call) \
({\
    bool f = flag;\
    if(f)\
        ImGui::PushStyleColor(ImGuiCol_Button, color);\
    bool textModified = call;\
    if(f)\
        ImGui::PopStyleColor();\
    textModified;\
})

using namespace GraphAnalyze;

GrapherModule::GrapherModule(int windowWidth, int windowHeight) : w(windowWidth), h(windowHeight), ism(this)
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
    try
    {
        p.SetExpr(std::string(buf));
        invalidFunc = false;
        evaluateFunction();
        gi.build(xs, ys);
    }
    catch(mu::Parser::exception_type &e)
    {
        invalidFunc = true;
        gi.ready  = false;
    }
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

std::string toString(double v, int precision = 0)
{
    std::ostringstream sstream;
    if(precision > 0)
        sstream << std::setprecision(precision);
    sstream << v;
    return sstream.str();
}

inline float clamp(float v, float a, float b)
{
    return std::max(a, std::min(b, v));
}

/**
 * Draws the built function using the current graph info.
 */
void GrapherModule::plotFunction(int w, int h)
{
    gi.updateArea(w, h);
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    ImVec2 top = gi.pos, bot(gi.pos.x + gi.size.x, gi.pos.y + gi.size.y),
    origin = gi.scale(0, 0);
    
    drawList->AddRectFilled(top, bot, 0xffffffff);
    
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
    int nbTicks = std::max(1, (w / xTickSpace) & ~1);
    for(int k = 0; k <= nbTicks; k++)
    {
        int x = top.x + w * k / nbTicks;
        // Avoid tick overlap
        if(abs(x - origin.x) >= xTickSpace)
        {
            drawList->AddLine(ImVec2(x, origin.y - 5), ImVec2(x, origin.y + 5), col32, 1);
            std::string s = toString(gi.minX + xrange * k / nbTicks, 2);
            ImVec2 textSize = ImGui::CalcTextSize(s.c_str());
            drawList->AddText(ImVec2(x - textSize.x / 2, clamp(origin.y + 5, top.y, bot.y - textSize.y)),
                col32, s.c_str());
        }
    }
    
    // Draw the Y axis' ticks
    if(yrange > 0)
    {
        nbTicks = std::max(1, (h / yTickSpace) & ~1);
        for(int k = 0; k <= nbTicks; k++)
        {
            int y = bot.y - h * k / nbTicks;
            // Avoid tick overlap
            if(abs(y - origin.y) >= yTickSpace)
            {
                drawList->AddLine(ImVec2(origin.x - 5, y), ImVec2(origin.x + 5, y), col32, 1);
                std::string s = toString(gi.minY + yrange * k / nbTicks, 2);
                ImVec2 textSize = ImGui::CalcTextSize(s.c_str());
                drawList->AddText(ImVec2(clamp(origin.x + 2, top.x, bot.x - textSize.x), y), col32, s.c_str());
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
}

/**
 * Handles the user clicking and dragging to zoom on a specific X range.
 * /!\ This needs the invisible button over the graph area to be the last drawn widget.
 */
void GrapherModule::handleZoom()
{
    static float startX, endX;
    if(userSelectArea(&startX, &endX, false, true) && startX != endX)
    {
        minX = gi.unscale(std::min(startX, endX), 0).x;
        maxX = gi.unscale(std::max(startX, endX), 0).x;
        refreshFunctionData();
    }
}

/**
 * Plots the tangent to the function at the position of the mouse.
 * /!\ This needs the invisible button over the graph area to be the last drawn widget.
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
 * Lets the user select an area in the graph by clicking and dragging with the left mouse button.
 * Returns true when the user is done selecting.
 * Optionally takes a flag telling whether the selected area should be drawn persistently in-between selects.
 * Optionally takes a function of two ImVec2 that handles drawing the selection. If omitted,
 * draws a standard semi-transparent green rectangle.
 * /!\ This needs the invisible button over the graph area to be the last drawn widget.
 */
bool GrapherModule::userSelectArea(float *startX, float *endX, bool persistent, bool allowOverlap, std::function<void(ImVec2&, ImVec2&)> selectionDrawer)
{
    static bool selecting = false;
    if(ImGui::IsItemHovered())
    {
        if(ImGui::IsMouseClicked(0))
        {
            selecting = true;
            *startX = *endX = ImGui::GetMousePos().x;
        }
        
        if(ImGui::IsMouseDown(0))
            *endX = allowOverlap ? ImGui::GetMousePos().x : std::max(*startX, ImGui::GetMousePos().x);
    }
    
    if(selecting || persistent)
    {
        ImDrawList *drawList = ImGui::GetWindowDrawList();
        
        ImVec2 rectMin = ImVec2(*startX, gi.pos.y),
            rectMax = ImVec2(*endX, gi.pos.y + gi.size.y);
        if(selectionDrawer)
            selectionDrawer(rectMin, rectMax);
        else
            drawList->AddRectFilled(rectMin, rectMax, 0x8800ff00);
    }
    
    if(selecting && !ImGui::IsMouseDown(0))
    {
        selecting = false;
        return true;
    }
    
    return false;
}

inline float lerp(float a, float b, float v)
{
    return (b - a) * v + a;
}

ImVec4 vlerp(ImVec4 a, ImVec4 b, float v)
{
    return ImVec4(lerp(a.x, b.x, v), lerp(a.y, b.y, v), lerp(a.z, b.z, v),
        lerp(a.w, b.w, v));
}

inline ImU32 clerp(ImVec4 &a, ImVec4 &b, float v)
{
    ImVec4 ha, hb;
    ImGui::ColorConvertRGBtoHSV(a.x, a.y, a.z, ha.x, ha.y, ha.z);
    ImGui::ColorConvertRGBtoHSV(b.x, b.y, b.z, hb.x, hb.y, hb.z);
    ha = vlerp(ha, hb, v);
    ImGui::ColorConvertHSVtoRGB(ha.x, ha.y, ha.z, ha.x, ha.y, ha.z);
    ha.w = lerp(a.w, b.w, v);
    return ImGui::ColorConvertFloat4ToU32(ha);
}

/**
 * Renders the module.
 */
void GrapherModule::render()
{
    const ImVec2 buttonSize = ImVec2(60, 30);
    
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
        if(ImGui::Button("Integrate", buttonSize))
            ism.active = true;
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
        int startPosGraph = ImGui::GetCursorPosX();
        static bool valueChanged = false;
        
        ImGui::PushItemWidth(windowW - startPosGraph - hSpacing * 2 - ImGui::CalcTextSize(" = f(x)").x);
            if(valueChanged |= flashWidget(invalidFunc, 0xff0000ff, ImGui::InputText(" = f(x)", buf, MAX_FUNC_LENGTH)))
                invalidFunc = false;
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
            if(flashButtonWidget(valueChanged, graphButtonColor, ImGui::Button("Graph")) && minX < maxX)
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
                    plotFunction(plotSize.x, plotSize.y);
                    // /!\ Obligatory invisible button right before the tangent
                    ImGui::InvisibleButton("PlotArea", plotSize);
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
