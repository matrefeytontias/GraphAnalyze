#include "graphWidget.h"

#include <iomanip>
#include <sstream>
#include <string>

inline float clamp(float v, float a, float b)
{
    return std::max(a, std::min(b, v));
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
