#include "imgui_user.h"
#include <iostream>
#include <cstring>


  ImVec2 operator+(ImVec2 a, ImVec2 b){
    return ImVec2(a.x+b.x,a.y+b.y);
  }

  ImVec2 operator-(ImVec2 a, ImVec2 b){
    return ImVec2(a.x-b.x,a.y-b.y);
  }

  struct ImGuiPlotArrayGetterData
  {
      const float* Values;
      int Stride;

      ImGuiPlotArrayGetterData(const float* values) { Values = values; Stride = 1; }
  };

namespace ImGui{
  void plotCustomHistogram(const char* overlay_text,std::vector<float>  values,std::vector<int> listName, int values_count, float scale_min, float scale_max, ImVec2 graph_size, std::string *selectedBranch)
  {
    float scrollX = ImGui::GetScrollX();
    float scrollY = ImGui::GetScrollY();
    ImVec2 scroll = ImVec2(scrollX,scrollY);

    float max = 0;
    for(unsigned int i=0;i<values.size();i++){
      if(values[i]>max)
        max = values[i];
    }

    ImGui::SetCursorPos(ImVec2(10,ImGui::GetCursorPosY()));
    ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(0.f,0.f,0.f,1.f));
    ImGui::Text(" %05.3f",max);
    ImGui::PopStyleColor();
    listName = listName;
      ImGuiWindow* window = ImGui::GetCurrentWindow();
      if (window->SkipItems)
          return;

      const ImGuiStyle& style =  ImGui::GetStyle();
      ImVec4 color;
     const ImVec4 col_base = ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram];

      const ImRect frame_bb(window->DC.CursorPos  , window->DC.CursorPos + ImVec2(graph_size.x, graph_size.y) );
      const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
      const ImRect total_bb(frame_bb.Min, frame_bb.Max);

      ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

      int res_w = ImMin((int)graph_size.x, values_count);
      const float t_step = 1.0f / (float)res_w;

      float v0 = values[0];
      float t0 = 0.0f;
      ImVec2 tp0 = ImVec2( t0, 1.0f - ImSaturate((v0 - scale_min) / (scale_max - scale_min)) );    // Point in the normalized space of our target rectangle

      for (int n = 0; n < res_w; n++)
      {
          const float t1 = t0 + t_step;
          const float v1 = values[n] ;

          const ImVec2 tp1 = ImVec2( t1, 1.0f - ImSaturate((v1 - scale_min) / (scale_max - scale_min)) );

          // NB: Draw calls are merged together by the DrawList system. Still, we should render our batch are lower level to save a bit of CPU.
          ImVec2 pos0 = ImLerp(inner_bb.Min, inner_bb.Max, tp0);
          ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, ImVec2(tp1.x, 1.0f));


              if (pos1.x >= pos0.x + 2.0f)
                  pos1.x -= 1.0f;
                int posY = pos1.y - graph_size.y * v1 / max;
                pos0.y = posY;

                ImGui::SetCursorPos(pos0 -ImGui::GetWindowPos() + scroll);

                if((std::to_string(listName[n]).compare(*selectedBranch))==0)
                    color = ImVec4(.1f,1.0f,.1f,1.f);
              else
                  color = col_base ;

                ImGui::PushID(listName[values_count-1]+n);
                ImGui::PushStyleColor(ImGuiCol_Button,color);;
                if(ImGui::Button(std::to_string(listName[n]).c_str(),  ImVec2(graph_size.x * t_step,graph_size.y * v1 / max))){
                 *selectedBranch = std::to_string(listName[n]);
              }
                ImGui::PopStyleColor();
                ImGui::PopID();
                if(IsItemHovered())
                  SetTooltip( "Chemin %i  \n + proba %f",listName[n],v1);
          t0 = t1;
          tp0 = tp1;
      }

      // Text overlay
      if (overlay_text)
        ImGui::RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, overlay_text, NULL, NULL, ImVec2(0.5f,0.0f));
  }


  void customHistogram(const char* overlay_text, std::vector<float>  values, int values_count,std::vector<int> listName, ImVec2 graph_size, std::string *selectedBranch)
  {
      plotCustomHistogram(overlay_text,values, listName, values_count, 0.f, 1.0f, graph_size,selectedBranch);
  }


}
