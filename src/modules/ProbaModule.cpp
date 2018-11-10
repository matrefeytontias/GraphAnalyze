#include "modules.h"
#include "imgui_user.h"

#include <algorithm>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <vector>
#include <cmath>
#include <queue>

using namespace GraphAnalyze;

bool *openProbaModule;
int repetition = 1;
int possibility = 2;
int selectedPossibility = 1;
std::string selectedNode = " ";
std::vector<float> histogramData;
std::vector<float> histoData2;
std::vector<int>  nameData2;
std::vector<float> proba;
ProbaModule::ProbaModule(bool state_arg[])
{
    openProbaModule = state_arg;
    for(int i =0; i< possibility; i++)
    {
        proba.push_back((float)1/(float)possibility);
    }
}

int addCount(int s)
{
    s++;
    if( s <=possibility )
        return s;
    else if(s == (possibility +1) )
        return 11;
    int mod = 10;

    while(mod <= s)
    {
        int r = s%mod;
        int u = 1;
        while(r >=10)
        {
            r = r/10;
            u = u*10;
        }
        if((r) == (possibility+1))
        {
            s=  (s-possibility*u) +mod;
        }
        mod = mod*10;
    }
    if((s/(mod/10)) == (possibility+1))
    {
        s=mod;
        while(mod>0)
        {
            mod = mod/10;
            s+=mod;
        }
    }
    return s;
}

int  factorial(int n)
{
    if(n<=0)
        return 1;
    return n * factorial(n-1);
}

float coefBino(float n, float k )
{
    if(  n == k)
        return 1.f;
    if(k <= 0 )
        return 1.f;
    return factorial(n)/((factorial (k))*(factorial(n-k)));
}


void setupField()
{
    float somme = 0;
    for(int i=0; i<possibility; i++)
    {
        somme += proba[i];
    }
    ImGui::InputInt("Number of repetition",&repetition);

    ImVec4 color;
    if(somme == 1.0f)
        color = ImVec4(.0f,.0f,.0f,1.f);
    else
        color = ImVec4(1.0f,.0f,.0f,1.f);

    ImGui::PushStyleColor(ImGuiCol_Text,color );
    if(ImGui::InputInt("Number of possibility",&possibility))
    {
        for(int i =0; i< possibility; i++)
        {
            if(possibility>(int)proba.size())
                proba.push_back(1/possibility);
            for(int i =0; i< possibility; i++)
            {
                proba[i] = ((float)1/(float)possibility);
            }
        }
    }
    float possibilitySize = ImGui::CalcTextSize("x.xxx").x + ImGui::CalcTextSize("Probability of possibility x").x;
    ImGui::PushItemWidth(possibilitySize);                                // width of items for the common item+label case, pixels. 0.0f = default to ~2/3 of windows width, >0.0f: width in pixels, <0.0f align xx pixels to the right of window (so -1.0f always align width to the right side)
    for(int i=0; i<possibility; i++)
    {
        ImGui::InputFloat(("Probability of possibility " + std::to_string(i+1)).c_str(),&proba[i],.0f,.0f,3);
        ImGui::SameLine();
    }
    ImGui::PopItemWidth();

    ImGui::Text("Somme : %f",somme);
    ImGui::PopStyleColor();
    ImGui::Separator();
    if(somme == 1.0f)
    {
        ImGui::InputInt("Select possibility",&selectedPossibility);
        if(selectedPossibility > 0 && selectedPossibility <= possibility)
        {
            histogramData.clear();
            for(int i =0; i<=repetition; i++)
            {
                histogramData.push_back(coefBino(repetition,i)*pow(proba[selectedPossibility-1],i) * pow((1-proba[selectedPossibility-1]),(repetition-i)));
            }
            ImGui::PlotHistogram("Proba", &(histogramData[0]), histogramData.size(),0,NULL,.0f,1.f,ImVec2(500,200));
        }
    }
}

ImVec2 nodeSize = ImVec2(20,20);
ImVec2 nodeSpacing = ImVec2(80,150);
int posY;

void addNode(int level,int number, ImVec2 treeSize,ImVec2 fullTreeSize,int numberNode,int count )
{
    treeSize = treeSize;
    level = level;
    ImGui::SetCursorPosX(nodeSize.x * level + nodeSpacing.x * (level - 1));
    int spacing = fullTreeSize.y / (numberNode+1);
    ImGui::SetCursorPosY(posY + spacing * (number+1));
    const char *name = std::to_string(count).c_str();
    if(count ==0)
        name = (const char*)((std::string)("")).c_str();
    ImVec4 baseColor = ImGui::GetStyle().Colors[ImGuiCol_Button];
    ImVec4 color = baseColor;

    std::string tempName(name);
    float totalProba = 1;

    for(unsigned int i =0 ; i<strlen(name); i++)
    {
        if(name[i]!=' ')
        {
            totalProba = totalProba * proba[atoi(&name[i])%10-1];
        }
    }

    for(unsigned int i = 0 ; i<=selectedNode.size(); i++)
    {
        if(selectedNode.substr(0,i).compare(tempName) ==0)
        {
            color = ImVec4(0.1f, 0.80f, 0.10f, 0.7f);
        }
    }
    name = tempName.c_str(); // ?
    ImGui::PushStyleColor(ImGuiCol_Button,color );
    if(ImGui::Button(name,nodeSize))
    {
        selectedNode = name;
    }
    ImGui::SameLine();
    if((level+1) == repetition)
    {
        ImGui::Text("%f",totalProba);
        histoData2.push_back(totalProba);
        nameData2.push_back(count);
    }
    ImGui::PopStyleColor();
}

void setupTree()
{

    int count = 0;
    std::queue<ImVec2> lastLevel;
    posY = ImGui::GetCursorPosY();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 fullTreeSize = ImVec2(repetition *nodeSize.x + (repetition-1)*nodeSpacing.x,
                                 pow(possibility,repetition-1) * nodeSize.y);
    ImVec2 treeSize;
    ImVec2 originPos;
    ImVec2 endPos;

    float scrollX = ImGui::GetScrollX();
    float scrollY = ImGui::GetScrollY();

    histoData2.clear();
    nameData2.clear();
    for(int i = 0 ; i< repetition; i++)
    {
        ImVec2 treeSize = ImVec2(i *nodeSize.x + (i-1)*nodeSpacing.x,
                                 pow(possibility,i-1) * nodeSize.y);
        int numberNode = pow(possibility,i-1);
        for(int j=0; j<numberNode; j++)
        {
            addNode(i,j,treeSize,fullTreeSize,numberNode,count);
            count = addCount(count);
            ImGui::SetCursorPosY(posY);

            endPos = ImVec2(nodeSize.x * i + nodeSpacing.x * (i - 1) + windowPos.x,
                            posY + (fullTreeSize.y / (numberNode+1)) * (j+1) + windowPos.y + nodeSize.y/2);
            for(int k=0; k<possibility; k++)
            {
                ImVec2 lastLevelPos = ImVec2(endPos.x +nodeSize.x,endPos.y);
                lastLevel.push(lastLevelPos);
            }
            if(i>1)
            {
                ImVec2 startLine = lastLevel.front();
                ImVec2 endLine = endPos;
                startLine.y -= scrollY;
                endLine.y -= scrollY;
                startLine.x -= scrollX;
                endLine.x -= scrollX;
                ImGui::GetWindowDrawList()->AddLine(startLine,endLine,ImGui::ColorConvertFloat4ToU32(ImVec4(1.f,1.f,1.f,.5f)));
                ImGui::SetCursorPosX(lastLevel.front().x - windowPos.x - (lastLevel.front().x - endPos.x)/2);
                ImGui::SetCursorPosY(lastLevel.front().y - windowPos.y - (lastLevel.front().y - endPos.y)/2);
                ImGui::Text("%f",proba[count%10 -1]);
                lastLevel.pop();
            }
        }
    }
    nodeSize = ImVec2((int)ImGui::CalcTextSize(std::to_string(count).c_str()).x+20,nodeSize.y);

}


void ProbaModule::render(std::string name)
{
    if(!ImGui::Begin(name.c_str(),openProbaModule,ImGuiWindowFlags_HorizontalScrollbar))
    {
        ImGui::End();
        return;
    }
    setupField();
    ImGui::Separator();


    if(repetition > 0 && possibility > 0 ){
    if(histoData2.size()>0)
      ImGui::customHistogram("Proba of each branch",  &(histoData2[0]), histoData2.size(),&(nameData2[0]), ImVec2(500.f,200.f),&selectedNode);

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,50.f);

        setupTree();
    ImGui::PopStyleVar();
  }

    ImGui::End();
}
