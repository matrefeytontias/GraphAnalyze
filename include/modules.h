#ifndef INC_MODULES
#define INC_MODULES

#include <algorithm>
#include <numeric>
#include <vector>

#include "imgui.h"
#include "mu/muParser.h"

#include "widgets.h"

namespace GraphAnalyze
{

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
    virtual void render() override;
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
    virtual void render() override;
private:
    bool *state;
};

#define MAX_DIFFEQ_DEGREE 10

class DiffEqSolverModule : public Module
{
public:
    DiffEqSolverModule(bool *open, int windowWidth = 640, int windowHeight = 480)
        : open(open), w(windowWidth), h(windowHeight)
    {
        for(unsigned int k = 0; k < MAX_DIFFEQ_DEGREE - 1; k++)
            aParsers[k].DefineVar("x", &x);
        bParser.DefineVar("x", &x);
    }
    virtual void render() override;
private:
    void solveDiffEq(double boundaryX, double minX, double maxX, double dx);
    double x;
    unsigned int degree = 1;
    char aBufs[MAX_DIFFEQ_DEGREE][MAX_FUNC_LENGTH] = { { 0 } };
    char bBuf[MAX_FUNC_LENGTH] = { 0 };
    bool invalids[MAX_DIFFEQ_DEGREE - 1] = { false };
    bool bInvalid = false;
    mu::Parser aParsers[MAX_DIFFEQ_DEGREE - 1];
    mu::Parser bParser;
    double boundaryYs[MAX_DIFFEQ_DEGREE - 1] = { 0 };
    bool *open;
    GraphInfo gi;
    int w, h;
    std::vector<double> xs, ys;
};

}

#endif
