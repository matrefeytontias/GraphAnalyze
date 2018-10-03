#include "modules.h"

#include <algorithm>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <vector>

using namespace GraphAnalyze;

std::vector<bool> *state;
HomeModule::HomeModule(std::vector<bool> state_arg)
{
    state = state_arg;
}

void HomeModule::render(){
       ImGui::Begin("Home");
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImGui::SetCursorPosX((windowSize.x - ImGui::CalcTextSize("Welcome").x)/2);
        ImGui::Text("Welcome");
        ImVec2 sizeButton = ImVec2(50,30);
        ImVec2 spacing = ImGui::GetStyle().ItemSpacing;
        ImGui::SetCursorPosX((windowSize.x - sizeButton.x *2 -spacing.x)/2);

        if(ImGui::Button("Open",sizeButton)){

        }
        ImGui::SameLine();
        if(ImGui::Button("Exit",sizeButton)){
            exit(0);
        }
        ImGui::Separator();
        ImGui::NewLine();
        ImVec2 textSize = ImGui::CalcTextSize("Plot Graph and show \n information about it");
        ImGui::SetCursorPosX((windowSize.x - textSize.x *2 - spacing.x)/2);
        ImVec2 sizeButtonModule = ImVec2(textSize.x,windowSize.y/10);
        ImGui::SetCursorPosY((windowSize.y - sizeButtonModule.y * 2 - textSize.y*2 - spacing.y * 3)/2);
        ImGui::BeginGroup();  //module list begin
            ImGui::BeginGroup(); //first column
                ImGui::BeginGroup(); //first module
                    if(ImGui::Button("Graph",sizeButtonModule))
                        state[0] = true;
                    ImGui::Text("Plot Graph and show \n information about it");
                ImGui::EndGroup(); //end first module
                ImGui::BeginGroup(); //second module
                    if(ImGui::Button("Module2",sizeButtonModule))
                        state[1]=true;
                    ImGui::Text("Description module2 \n ...");
                ImGui::EndGroup(); //end second module
            ImGui::EndGroup();  //End first column
            ImGui::SameLine();
            ImGui::BeginGroup(); //2nd column
                ImGui::BeginGroup(); //first module
                    if(ImGui::Button("Module3",sizeButtonModule))
                        state[2]=true;
                    ImGui::Text("Description module3 \n ...");
                ImGui::EndGroup(); //end first module
                ImGui::BeginGroup(); //second module
                    if(ImGui::Button("Module4",sizeButtonModule))
                        state[3]=true;
                    ImGui::Text("Description module4 \n ...");
                ImGui::EndGroup(); //end second module
            ImGui::EndGroup();  //End 2nd column
        ImGui::EndGroup(); //module list end
    ImGui::End();

}
