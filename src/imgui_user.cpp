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

      ImGuiPlotArrayGetterData(const float* values, int stride) { Values = values; Stride = stride; }
  };

  float Plot_ArrayGetter(void* data, int idx)
 {
     ImGuiPlotArrayGetterData* plot_data = (ImGuiPlotArrayGetterData*)data;
     const float v = *(const float*)(const void*)((const unsigned char*)plot_data->Values + (size_t)idx * plot_data->Stride);
     return v;
 }

namespace ImGui{
  void plotCustomHistogram(const char* overlay_text,float (*values_getter)(void* data, int idx), void* data,int* listName, int values_count, float scale_min, float scale_max, ImVec2 graph_size, std::string *selectedBranch)
  {
    listName = listName;
      ImGuiWindow* window = ImGui::GetCurrentWindow();
      if (window->SkipItems)
          return;

      const ImGuiStyle& style =  ImGui::GetStyle();
      ImVec4 color;
     const ImVec4 col_base = ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram];

     float scrollX = ImGui::GetScrollX();
     float scrollY = ImGui::GetScrollY();

     ImVec2 scroll = ImVec2(scrollX,scrollY);
     scroll=scroll;

      const ImRect frame_bb(window->DC.CursorPos  , window->DC.CursorPos + ImVec2(graph_size.x, graph_size.y) );
      const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
      const ImRect total_bb(frame_bb.Min, frame_bb.Max);

      ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

      int res_w = ImMin((int)graph_size.x, values_count);
      int item_count = values_count;

      const float t_step = 1.0f / (float)res_w;

      float v0 = values_getter(data, 0);
      float t0 = 0.0f;
      ImVec2 tp0 = ImVec2( t0, 1.0f - ImSaturate((v0 - scale_min) / (scale_max - scale_min)) );    // Point in the normalized space of our target rectangle

      for (int n = 0; n < res_w; n++)
      {
          const float t1 = t0 + t_step;
          const int v1_idx = (int)(t0 * item_count + 0.5f);
          IM_ASSERT(v1_idx >= 0 && v1_idx < item_count);
          const float v1 = values_getter(data, (v1_idx + 1) % item_count);
          const ImVec2 tp1 = ImVec2( t1, 1.0f - ImSaturate((v1 - scale_min) / (scale_max - scale_min)) );

          // NB: Draw calls are merged together by the DrawList system. Still, we should render our batch are lower level to save a bit of CPU.
          ImVec2 pos0 = ImLerp(inner_bb.Min, inner_bb.Max, tp0);
          ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, ImVec2(tp1.x, 1.0f));

              if (pos1.x >= pos0.x + 2.0f)
                  pos1.x -= 1.0f;
                int posY = pos1.y - graph_size.y * v1;
                pos0.y = posY;
                ImGui::SetCursorPos(pos0 -ImGui::GetWindowPos() + scroll);
                if((std::to_string(listName[n]).compare(*selectedBranch))==0)
                    color = ImVec4(.1f,1.0f,.1f,1.f);
              else
                  color = col_base ;

                ImGui::PushID(listName[values_count-1]+n);
                ImGui::PushStyleColor(ImGuiCol_Button,color);
                if(ImGui::Button(std::to_string(listName[n]).c_str(),  ImVec2(graph_size.x * t_step,graph_size.y * v1))){
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


  void customHistogram(const char* overlay_text, const float* values, int values_count,int* listName, ImVec2 graph_size, std::string *selectedBranch)
  {
      ImGuiPlotArrayGetterData data(values, sizeof(float));
      plotCustomHistogram(overlay_text,&Plot_ArrayGetter, (void*)&data, listName, values_count, 0.f, 1.0f, graph_size,selectedBranch);
  }


}
