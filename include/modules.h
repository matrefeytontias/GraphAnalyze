#ifndef INC_MODULES
#define INC_MODULES

#include <algorithm>
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

class Module
{
public:
    virtual ~Module() { }
    virtual void render() = 0;
    /**
     * Lets the module handle the user's mouse or not.
     */
    void canHandleClick(bool v)
    {
        hasClick = v;
    }
protected:
    bool hasClick = true;
};

template <class T> class SubModule : public Module
{
public:
    SubModule(T *parent) : parent(parent) { }
    bool active = false;
protected:
    T *parent;
};

#define PLOT_INTERVALS 5000
#define MAX_FUNC_LENGTH 5000

class GrapherModule;

class IntegrationSubModule : public SubModule<GrapherModule>
{
public:
    IntegrationSubModule(GrapherModule *parent) : SubModule(parent) { }
    virtual ~IntegrationSubModule() override { }
    virtual void render() override;
private:
    void selectionDrawer(float x1, float x2);
    float startX = -1, endX = 1;
};

class GrapherModule : public Module
{
    friend IntegrationSubModule;
public:
    GrapherModule(bool *open, int windowWidth = 640, int windowHeight = 480);
    virtual ~GrapherModule() override { }
    virtual void render() override;
    bool userSelectArea(float *startX, float *endX, bool persistent = false, bool allowOverlap = false,
        std::function<void(float, float)> selectionDrawer = nullptr);
private:
    bool *open;
    GraphInfo gi;
    void refreshFunctionData();
    void evaluateFunction();
    void plotFunction(int w, int h);
    void handleZoom();
    void plotTangent(float length = 50);
    int w, h;
    mu::Parser p;
    float minX = -1, maxX = 1;
    std::vector<double> xs, ys;
    double x = 0.;
    bool invalidFunc = false;
    char buf[MAX_FUNC_LENGTH] = "";
    IntegrationSubModule ism;
};

class HomeModule : public Module
{
public:
    HomeModule(bool *state_ptr) : state(state_ptr) { }
    virtual ~HomeModule() override { }
    virtual void render() override;
private:
    bool *state;
};

class ProbaModule : public Module
{
public:
    ProbaModule(bool *b);
    virtual void render() override;
};


}

#endif
