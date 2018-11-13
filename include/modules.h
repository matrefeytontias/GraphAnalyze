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

/**
 * Differential equation solver module. Allows one to solve linear ordinary
 * differential equations up to the tenth degree.
 */
class DiffEqSolverModule : public Module
{
public:
    /**
     * Constructs the module.
     * @param   open            pointer to a boolean indicating whether the window should render
     * @param   windowWidth     width of the window
     * @param   windowHeight    height of the window
     */
    DiffEqSolverModule(bool *open, int windowWidth = 640, int windowHeight = 480)
        : open(open), w(windowWidth), h(windowHeight)
    {
        for(unsigned int k = 0; k < MAX_DIFFEQ_DEGREE; k++)
            aParsers[k].DefineVar("x", &x);
        bParser.DefineVar("x", &x);
    }
    virtual void render() override;
private:
    void solveDiffEq(double boundaryX, double minX, double maxX, double dx);
    /**
     * Parameter used for all the `mu::Parser` evaluations.
     */
    double x;
    /**
     * Degree of the current differential equations.
     */
    unsigned int degree = 1;
    /**
     * Character buffers holding the various `a` functions' expression.
     */
    char aBufs[MAX_DIFFEQ_DEGREE][MAX_FUNC_LENGTH] = { { 0 } };
    /**
     * Character buffer holding the `b` function's expression.
     */
    char bBuf[MAX_FUNC_LENGTH] = { 0 };
    /**
     * Tells whether one of the `a` functions' expression is invalid by its index.
     */
    bool invalids[MAX_DIFFEQ_DEGREE] = { false };
    /**
     * Tells whether the `b` function's expression is invalid.
     */
    bool bInvalid = false;
    /**
     * Parsers for the `a` functions.
     */
    mu::Parser aParsers[MAX_DIFFEQ_DEGREE];
    /**
     * Parser for the `b` function.
     */
    mu::Parser bParser;
    /**
     * Array of boundary conditions for the derivatives of `y`.
     */
    double boundaryYs[MAX_DIFFEQ_DEGREE] = { 0 };
    /**
     * Tells whether the window should render.
     */
    bool *open;
    /**
     * Graph info associated to the solution of the differential equation.
     */
    GraphInfo gi;
    /**
     * Window dimensions.
     */
    int w, h;
    /**
     * Abscissa and ordinate of the solution.
     */
    std::vector<double> xs, ys;
};

}

#endif
