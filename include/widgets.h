#ifndef INC_GRAPH_WIDGET
#define INC_GRAPH_WIDGET

#include <functional>
#include <numeric>
#include <vector>

#include "imgui.h"
#include "mu/muParser.h"

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

/**
 * Draws an interactive graph of the graph info and function values given, with the
 * dimensions given.
 */
void GraphWidget(GraphInfo &gi, std::vector<double> &xs, std::vector<double> &ys,
    int w, int h);

/**
 * Lets the user select an area in a graph widget by clicking and dragging with
 * the left mouse button. Writes coordinates in function space. Returns true when
 * the user is done selecting.
 * Optionally takes a flag telling whether the selected area should be drawn persistently
 * in-between selects.
 * Optionally takes a function of two floats that handles drawing the selection.
 * If omitted, draws a standard semi-transparent green rectangle.
 * /!\ This needs a graph widget to be the last drawn widget.
 */
bool userSelectArea(GraphInfo &gi, float *startX, float *endX, bool persistent = false,
    bool allowOverlap = false, std::function<void(float, float)> selectionDrawer = nullptr);

/**
 * A special InputText that queries a function that mu::Parser can recognize, and
 * writes it to the given parser. Returns whether or not its contents changed this
 * frame. Writes true to `invalid` if the widget is not active (ie being edited)
 * and the function is invalid, and false in all other cases (this is useful for
 * flashing the widget).
 * WARNING : be sure to set your parser variables to values that will allow the
 * parser to tell whether a function is valid or not.
 */
bool InputFunction(const char *label, char *buf, size_t size, mu::Parser &p, bool *invalid);

};

ImU32 clerp(ImVec4 &a, ImVec4 &b, float v);

#endif
