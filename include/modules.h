#ifndef INC_MODULES
#define INC_MODULES

#include <algorithm>
#include <numeric>
#include <vector>

#include "imgui.h"
#include "mu/muParser.h"

#include "widgets.h"

/**
 * Holds all GraphAnalyze modules and widgets.
 */
namespace GraphAnalyze
{

/**
 * Base class for modules.
 */
class Module
{
public:
    virtual ~Module() { }
    /**
     * Draws the module to the screen and handles its logic.
     */
    virtual void render() = 0;
    /**
     * Lets the module handle the user's mouse or not.
     */
    void canHandleClick(bool v)
    {
        hasClick = v;
    }
protected:
    /**
     * Whether the module should catch the mouse and not propagate it.
     */
    bool hasClick = true;
};

/**
 * Base class for submodules.
 */
template <class T> class SubModule : public Module
{
public:
    SubModule(T *parent) : parent(parent) { }
    /**
     * Whether the submodule should be rendered and subjected to input events.
     */
    bool active = false;
protected:
    T *parent;
};

#define PLOT_INTERVALS 5000
#define MAX_FUNC_LENGTH 5000

class GrapherModule;

/**
 * Numerical integration submodule for the GrapherModule.
 */
class IntegrationSubModule : public SubModule<GrapherModule>
{
public:
    IntegrationSubModule(GrapherModule *parent) : SubModule(parent) { }
    virtual void render() override;
private:
    /**
     * Custom callback for `GraphAnalyze::userSelectArea` to draw the area under
     * the active curve.
     */
    void selectionDrawer(float x1, float x2);
    /**
     * Start and end of the selection.
     */
    float startX = -1, endX = 1;
};

/**
 * Graphing, tangent plotting and numerical integration module.
 */
class GrapherModule : public Module
{
    friend IntegrationSubModule;
public:
    GrapherModule(bool *open, int windowWidth = 640, int windowHeight = 480);
    virtual void render() override;
private:
    bool *open;
    GraphInfo gi;
    /**
     * Reapplies the contents of the function to the graph info and the coordinate
     * arrays.
     */
    void refreshFunctionData();
    /**
     * Reapplies the contents of the function to the coordinate arrays.
     */
    void evaluateFunction();
    /**
     * Handles user selecting an area in the graph widget to zoom in.
     */
    void handleZoom();
    /**
     * Plots the tangent to the current function at the mouse's position.
     * @param   length  length of the tangent in pixels
     */
    void plotTangent(float length = 50);
    /**
     * Window dimensions.
     */
    int w, h;
    /**
     * Parser for the function's expression.
     */
    mu::Parser p;
    /**
     * Boundaries for the graphing range.
     */
    float minX = -1, maxX = 1;
    /**
     * Coordinate arrays for the function graph.
     */
    std::vector<double> xs, ys;
    /**
     * Parameter for the function's parser evaluations.
     */
    double x = 0.;
    /**
     * Tells whether the function's expression is invalid.
     */
    bool invalidFunc = false;
    /**
     * Character buffer for the function's expression.
     */
    char buf[MAX_FUNC_LENGTH] = "";
    /**
     * Child numerical integration submodule.
     */
    IntegrationSubModule ism;
};

/**
 * Home screen module. Dispatches the user to the different modules of GraphAnalyze.
 */
class HomeModule : public Module
{
public:
    HomeModule(bool *state_ptr) : state(state_ptr) { }
    virtual void render() override;
private:
    /**
     * Array of booleans telling which modules should be active.
     */
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
