#ifndef INC_MODULES
#define INC_MODULES

#include <vector>

#include "imgui.h"
#include "mu/muParser.h"

namespace GraphAnalyze
{

class Module
{
    virtual void render() = 0;
};

#define MAX_FUNC_LENGTH 5000

class GrapherModule : public Module
{
public:
    GrapherModule(int windowWidth = 640, int windowHeight = 480);
    virtual void render() override;
private:
    void refreshFunctionData();
    void evaluateFunction();
    void plotFunction(int w, int h);
    void handleZoom();
    void plotTangent(float length = 50);
    int w, h;
    mu::Parser p;
    // TODO : move this back to double
    float minX = -1, maxX = 1;
    std::vector<float> xs, ys;
    double x = 0.;
    bool invalidFunc = false;
    char buf[MAX_FUNC_LENGTH] = "";
};

}

#endif
