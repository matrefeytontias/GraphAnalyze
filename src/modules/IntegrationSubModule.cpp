#include "modules.h"

#include <sstream>
#include <vector>

#include "imgui.h"

#include "utils.h"

using namespace GraphAnalyze;

/**
 * Renders the submodule and performs action.
 * /!\ This needs the invisible button over the graph area in the GrapherModule
 * to be the last drawn widget.
 */
void IntegrationSubModule::render()
{
    parent->canHandleClick(!active);
    
    if(active)
    {
        // Select area first because we need the graph area to be the last drawn widget
        static float minX, maxX;
        parent->userSelectArea(&minX, &maxX, true, true, [=](ImVec2 &a, ImVec2 &b) { this->selectionDrawer(a, b); });
        startX = parent->gi.unscale(minX, 0).x;
        endX = parent->gi.unscale(maxX, 0).x;
        
        ImGui::Begin("Numerical integration", &active, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Please select boundaries in the graph area.");
        std::ostringstream ss;
        ss << "Integrating from " << startX << " to " << endX;
        ImGui::Text(ss.str().c_str());
        double rangeX = parent->gi.maxX - parent->gi.minX;
        std::vector<double> &xs = parent->xs,
            &ys = parent->ys;
        int minIndex = (int)((std::min(startX, endX) - parent->gi.minX) * PLOT_INTERVALS
            / rangeX),
            maxIndex = (int)((std::max(startX, endX) - parent->gi.minX) * PLOT_INTERVALS
            / rangeX);
        double result = 0;
        for(int k = minIndex; k < maxIndex; k++)
            result += (ys[k + 1] + ys[k]) * (xs[k + 1] - xs[k]) / 2;
        if(minX > maxX)
            result *= -1;
        ss.str("");
        ss << "Result : " << result;
        ImGui::Text(ss.str().c_str());
        ImGui::End();
    }
}

/**
 * Draws the selection of the integration bounds.
 */
void IntegrationSubModule::selectionDrawer(ImVec2 &rectMin, ImVec2 &rectMax)
{
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    std::vector<double> &ys = parent->ys;
    
    float originY = parent->gi.scale(0, 0).y,
        xmin = std::min(rectMin.x, rectMax.x),
        xmax = std::max(rectMin.x, rectMax.x);
    
    for(float x = xmin; x <= xmax; x += 1.f)
    {
        int index = (int)((x - parent->gi.pos.x) * PLOT_INTERVALS / parent->gi.size.x);
        float y = parent->gi.scale(0, ys[index]).y;
        drawList->AddLine(ImVec2(x, originY), ImVec2(x, y), 0x880088ff);
    }
}
