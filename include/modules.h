#ifndef INC_MODULES
#define INC_MODULES

#include <vector>

#include "imgui.h"
#include "mu/muParser.h"

namespace GraphAnalyze
{

class Module
{
public:
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

#define MAX_FUNC_LENGTH 5000

class GrapherModule : public Module
{
public:
    GrapherModule(int windowWidth = 640, int windowHeight = 480);
    virtual void render() override;
    bool userSelectArea(float *startX, float *endX, bool allowOverlap = false);
private:
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
};

}

#endif
