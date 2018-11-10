#include "imgui.h"
#include "imgui_internal.h"
#include <algorithm>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <vector>
#include <cmath>
#include <queue>

namespace ImGui{

  void customHistogram(const char* overlay_text, const float* values, int values_count,int* listName, ImVec2 graph_size, std::string *selectedBranch);
}
