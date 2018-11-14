#ifndef INC_GRAPH_WIDGET
#define INC_GRAPH_WIDGET

#include <functional>
#include <numeric>
#include <vector>

#include "imgui.h"
#include "mu/muParser.h"

namespace GraphAnalyze
{

/**
 * Structure holding information about the state of the graph of a function.
 */
typedef struct
{
    /**
     * Position of the graph in an ImGUI window.
     */
    ImVec2 pos;
    /**
     * Size of the graph.
     */
    ImVec2 size;
    double minX = -1., maxX = 1., minY, maxY;
    /**
     * Tells whether the structure contains enough data to plot a function.
     */
    bool ready = false;
    /**
     * Sets the drawing area. If width and height aren't provided, use all of the
     * available space.
     * @param   w   widget width
     * @param   h   widget height
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
     * Maps an (x, y) value from function space to graph space.
     */
    inline ImVec2 scale(double x, double y)
    {
        return ImVec2((x - minX) * (size.x - 1) / (maxX - minX) + pos.x,
            size.y - (y - minY) * (size.y - 1) / (maxY - minY) + pos.y);
    }
    
    /**
     * Maps an (x, y) value from function space to graph space.
     */
    inline ImVec2 scale(ImVec2 a)
    {
        return scale(a.x, a.y);
    }
    
    /**
     * Maps an (x, y) value from graph space to function space.
     */
    inline ImVec2 unscale(float x, float y)
    {
        return ImVec2((x - pos.x) * (maxX - minX) / (size.x - 1) + minX,
            (y - pos.y) * (maxY - minY) / (size.y - 1) + minY);
    }
    
    /**
     * Maps an (x, y) value from graph space to function space.
     */
    inline ImVec2 unscale(ImVec2 a)
    {
        return unscale(a.x, a.y);
    }
} GraphInfo;

/**
 * Draws an interactive graph of the graph info and function values.
 * @param   gi  GraphInfo structure to use. Must be valid
 * @param   xs  array of abscissae
 * @param   ys  array of ordinates
 * @param   w   widget width
 * @param   h   widget height
 */
void GraphWidget(GraphInfo &gi, std::vector<double> &xs, std::vector<double> &ys,
    int w, int h);

/**
 * Lets the user select an area in a graph widget by clicking and dragging with
 * the left mouse button. Writes coordinates in function space.
 * /!\ This needs a graph widget to be the last drawn widget.
 * @param   gi              GraphInfo structure to use. Must be valid
 * @param   startX          where to write the start of the selected range
 * @param   endX            where to write the end of the selected range
 * @param   persistent      whether the selected area should be drawn persistently in-between selects
 * @param   allowOverlap    whether to allow startX > endX
 * @param   selectionDrawer function of two floats that handles drawing the selection.
                            If omitted, draws a standard semi-transparent green rectangle
 * @return  true when the user is done selecting
 */
bool userSelectArea(GraphInfo &gi, float *startX, float *endX, bool persistent = false,
    bool allowOverlap = false, std::function<void(float, float)> selectionDrawer = nullptr);

/**
 * A special InputText that queries a function that mu::Parser can recognize, and
 * writes it to the given parser. WARNING : be sure to set your parser variables
 * to values that will allow the parser to tell whether a function is valid or not.
 * @param   label   label/id of the widget
 * @param   buf     character buffer where to hold the inputted text
 * @param   size    maximum size of the inputted text
 * @param   p       parser to use when testing for validity of expression
 * @param   invalid whether the widget is inactive *and* the function invalid
 * @return  whether or not the widget's contents changed this frame
 */
bool InputFunction(const char *label, char *buf, size_t size, mu::Parser &p, bool *invalid);

};

/**
 * Linear interpolation of `ImVec4` colors through HSV space as `ImU32`.
 * @param   a   start color
 * @param   b   end color
 * @param   v   interpolation factor
 */
ImU32 clerp(ImVec4 &a, ImVec4 &b, float v);

#endif
