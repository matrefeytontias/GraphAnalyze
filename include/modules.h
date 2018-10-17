#ifndef INC_MODULES
#define INC_MODULES

#include <vector>

#include "imgui.h"
#include "mu/muParser.h"

namespace GraphAnalyze
{

class Module
{
    virtual void render(std::string s) = 0;
};

#define MAX_FUNC_LENGTH 5000

class GrapherModule : public Module
{
public:
    GrapherModule(bool *b, int windowWidth = 640, int windowHeight = 480);
    virtual void render(std::string s) override;
private:
    void evaluateFunction(double minX, double maxX);
    void plotFunction(int w, int h);
    void plotTangent(float length = 50);
    int w, h;
    mu::Parser p;
    // TODO : move this back to double
    std::vector<float> xs, ys;
    double x = 0.;
    bool invalidFunc = false;
    char buf[MAX_FUNC_LENGTH] = "";
};

class HomeModule : public Module{
public:
    HomeModule(bool state[]);
    virtual void render(std::string s) override;
};

}

#endif
