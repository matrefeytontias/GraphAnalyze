#ifndef INC_GRAPH_WIDGET
#define INC_GRAPH_WIDGET

#include <numeric>
#include <vector>

#include "imgui.h"

namespace GraphAnalyze
{

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
        return ImVec2((x - minX) * (size.x - 1) / (maxX - minX) + pos.x,
            size.y - (y - minY) * (size.y - 1) / (maxY - minY) + pos.y);
    }
    
    inline ImVec2 scale(ImVec2 a)
    {
        return scale(a.x, a.y);
    }
    
    /**
     * Maps an (x, y) value in graph space to function space.
     */
    inline ImVec2 unscale(float x, float y)
    {
        return ImVec2((x - pos.x) * (maxX - minX) / (size.x - 1) + minX,
            (y - pos.y) * (maxY - minY) / (size.y - 1) + minY);
    }
    
    inline ImVec2 unscale(ImVec2 a)
    {
        return unscale(a.x, a.y);
    }
} GraphInfo;

void GraphWidget(GraphInfo &gi, std::vector<double> &xs, std::vector<double> &ys,
    int w, int h);

};

ImU32 clerp(ImVec4 &a, ImVec4 &b, float v);

#endif
