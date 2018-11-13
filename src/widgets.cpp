#include "widgets.h"

#include <iomanip>
#include <sstream>
#include <string>

#include "utils.h"

inline float clamp(float v, float a, float b)
{
    return std::max(a, std::min(b, v));
}

inline float lerp(float a, float b, float v)
{
    return (b - a) * v + a;
}

inline ImVec4 vlerp(ImVec4 a, ImVec4 b, float v)
{
    return ImVec4(lerp(a.x, b.x, v), lerp(a.y, b.y, v), lerp(a.z, b.z, v),
        lerp(a.w, b.w, v));
}

ImU32 clerp(ImVec4 &a, ImVec4 &b, float v)
{
    ImVec4 ha, hb;
    ImGui::ColorConvertRGBtoHSV(a.x, a.y, a.z, ha.x, ha.y, ha.z);
    ImGui::ColorConvertRGBtoHSV(b.x, b.y, b.z, hb.x, hb.y, hb.z);
    ha = vlerp(ha, hb, v);
    ImGui::ColorConvertHSVtoRGB(ha.x, ha.y, ha.z, ha.x, ha.y, ha.z);
    ha.w = lerp(a.w, b.w, v);
    return ImGui::ColorConvertFloat4ToU32(ha);
}

std::string toString(double v, int precision = 0)
{
    std::ostringstream sstream;
    if(precision > 0)
        sstream << std::setprecision(precision);
    sstream << v;
    return sstream.str();
}

void GraphAnalyze::GraphWidget(GraphInfo &gi, std::vector<double> &xs, std::vector<double> &ys,
    int w, int h)
{
    gi.updateArea(w, h);
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    ImVec2 top = gi.pos, bot(gi.pos.x + gi.size.x, gi.pos.y + gi.size.y),
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
    for(int k = 0; k + 1 < w; k++)
    {
        unsigned int i = std::lower_bound(xs.begin(), xs.end(), gi.unscale(k + gi.pos.x, 0).x) - xs.begin(),
            j = std::lower_bound(xs.begin(), xs.end(), gi.unscale(k + 1 + gi.pos.x, 0).x) - xs.begin();
        if(i < xs.size() && j < ys.size())
            drawList->AddLine(gi.scale(xs[i], ys[i]), gi.scale(xs[j], ys[j]),
                col32, 1);
    }
    
    ImGui::PopClipRect();
    // Make the widget react like an actual ImGui widget wrt interaction
    ImGui::InvisibleButton("PlotArea", ImVec2(w, h));
}

bool GraphAnalyze::userSelectArea(GraphInfo &gi, float *startX, float *endX,
    bool persistent, bool allowOverlap, std::function<void(float, float)> selectionDrawer)
{
    static bool selecting = false;
    
    if(*startX < gi.minX)
        *startX = gi.minX;
    if(*endX > gi.maxX)
        *endX = gi.maxX;
    
    if(ImGui::IsItemHovered())
    {
        if(ImGui::IsMouseClicked(0))
        {
            selecting = true;
            *startX = *endX = gi.unscale(ImGui::GetMousePos()).x;
        }
        
        if(ImGui::IsMouseDown(0))
            *endX = allowOverlap ? gi.unscale(ImGui::GetMousePos()).x
                : std::max(*startX, gi.unscale(ImGui::GetMousePos()).x);
    }
    
    if(selecting || persistent)
    {
        ImDrawList *drawList = ImGui::GetWindowDrawList();
        
        ImVec2 rectMin = gi.scale(*startX, gi.maxY),
            rectMax = gi.scale(*endX, gi.minY);
        if(selectionDrawer)
            selectionDrawer(rectMin.x, rectMax.x);
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

bool GraphAnalyze::InputFunction(const char *label, char *buf, size_t size, mu::Parser &p, bool *invalid)
{
    bool valueChanged = ImGui::InputText(label, buf, size);
    *invalid = false;
    if(!ImGui::IsItemActive() && buf[0] != '\0')
    {
        try
        {
            p.SetExpr(std::string(buf));
            p.Eval();
        }
        catch(mu::Parser::exception_type &e)
        {
            *invalid = true;
        }
    }
    
    return valueChanged;
}
