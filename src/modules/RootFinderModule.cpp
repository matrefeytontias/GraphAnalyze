#include "modules.h"

#include <algorithm>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <vector>

using namespace GraphAnalyze;

bool *openRootModule;
RootFinderModule::RootFinderModule(bool *b)
{
    openRootModule = b;
}

void RootFinderModule::render(std::string name){
  if(!ImGui::Begin(name.c_str(),openRootModule)){
      ImGui::End();
      return;
  }
    ImVec2 windowSize = ImGui::GetWindowSize();
//    ImVec2 spacing = ImGui::GetStyle().ItemSpacing;
    ImGui::SetCursorPosX((windowSize.x - ImGui::CalcTextSize("Root Finder").x)/2);
    ImGui::Text("Root Finder");

    ImGui::Text("Formula");
    char buf[100];
    ImGui::SameLine();
    ImGui::InputText(" = f(x)",buf,100);
    ImGui::Separator();
    ImGui::NewLine();

    ImGui::SetCursorPosX((windowSize.x - ImGui::CalcTextSize("Roots").x)/2);
    ImGui::Text("Roots");
    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::NewLine();
    // ...
    ImGui::Separator();
    ImGui::SetCursorPosX((windowSize.x - ImGui::CalcTextSize("Graph").x)/2);
    ImGui::Text("Graph");
    float tab[] = {1.f,2.f,30.f};
    ImGui::PlotLines("",tab,3);

  ImGui::End();
}
