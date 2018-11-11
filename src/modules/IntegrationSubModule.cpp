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
        parent->userSelectArea(&startX, &endX, true, true, [=](float a, float b) { this->selectionDrawer(a, b); });

        ImGui::Begin("Numerical integration", &active, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Please select boundaries in the graph area.");
        std::ostringstream ss;
        ss << "Integrating from " << startX << " to " << endX;
        ImGui::Text("%s",ss.str().c_str());
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
        if(startX > endX)
            result *= -1;
        ss.str("");
        ss << "Result : " << result;
        ImGui::Text("%s",ss.str().c_str());
        ImGui::End();
    }
}

/**
 * Draws the selection of the integration bounds.
 */
 void IntegrationSubModule::selectionDrawer(float x1, float x2)
{
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    std::vector<double> &ys = parent->ys;

    float originY = parent->gi.scale(0, 0).y,
        xmin = std::min(x1, x2),
        xmax = std::max(x1, x2);

    for(float x = xmin; x <= xmax; x += 1.f)
    {
        int index = (int)((x - parent->gi.pos.x) * PLOT_INTERVALS / parent->gi.size.x);
        float y = parent->gi.scale(0, ys[index]).y;
        drawList->AddLine(ImVec2(x, originY), ImVec2(x, y), 0x880088ff);
    }
}
