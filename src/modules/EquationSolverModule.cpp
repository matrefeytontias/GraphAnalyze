#include "modules.h"

#include <algorithm>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <vector>

using namespace GraphAnalyze;

bool *openSolverModule;
EquationSolverModule::EquationSolverModule(bool *b)
{
    openSolverModule = b;
}

void EquationSolverModule::render(std::string name){
  if(!ImGui::Begin(name.c_str(),openSolverModule)){
      ImGui::End();
      return;
  }
    ImVec2 windowSize = ImGui::GetWindowSize();
  //  ImVec2 spacing = ImGui::GetStyle().ItemSpacing;
    ImGui::SetCursorPosX((windowSize.x - ImGui::CalcTextSize("Solver").x)/2);
    ImGui::Text("Solver");
    ImGui::NewLine();
    ImGui::Text("Equation");
    char buf[100];
    ImGui::SameLine();
    ImGui::InputText("",buf,100);
    ImGui::Separator();
    ImGui::NewLine();
    ImGui::SetCursorPosX((windowSize.x - ImGui::CalcTextSize("Result").x)/2);
    ImGui::Text("Result");
    // ...


  ImGui::End();
}
