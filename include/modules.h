#ifndef INC_MODULES
#define INC_MODULES

#include <vector>

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
    int w, h;
    mu::Parser p;
    std::vector<double> xs, ys;
    double x = 0.;
    bool readyToGraph = false;
    bool invalidFunc = false;
    char buf[MAX_FUNC_LENGTH] = "x^2 + x + 1";
};

}

#endif
