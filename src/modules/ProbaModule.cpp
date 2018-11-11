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

const int MAX_REPETITION = 8;
const int MAX_POSSIBILITE = 4;
bool *openProbaModule;
int repetition = 0;
int possibility = 2;
int selectedPossibility = 1;
bool showHistogram = false;
bool showHistoTree = false;
bool showTree = false;
bool showField = true;
float somme = 1;
std::string selectedNode = " ";
std::vector<float> histogramData;
std::vector<float> histoData2;
std::vector<int>  nameData2;
std::vector<float> proba;

ProbaModule::ProbaModule(bool *state_arg)
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
     somme = 0;
    for(int i=0; i<possibility; i++)
    {
        somme += proba[i];
    }
    ImGui::InputInt("Number of repetition",&repetition);
    if(repetition < 0 )
        repetition = 0;
    else if(repetition > MAX_REPETITION)
        repetition = MAX_REPETITION;

    ImVec4 color;
    if(somme -  1.0f < 0.0001f)
        color = ImVec4(.0f,.0f,.0f,1.f);
    else
    {
        color = ImVec4(1.0f,.0f,.0f,1.f);
    }
//    std::cout<<somme<<std::endl;
    ImGui::PushStyleColor(ImGuiCol_Text,color );
    if(ImGui::InputInt("Number of possibility",&possibility))
    {
        if(possibility < 1 )
            possibility = 1;
        else if (possibility > MAX_POSSIBILITE)
            possibility = MAX_POSSIBILITE;
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
    float possibilitySize = ImGui::CalcTextSize("0.0000").x;
    float textSize = ImGui::CalcTextSize("Probability of possibility x").x;
    ImGui::PushItemWidth(possibilitySize);
    int lineSize = ImGui::GetCursorPosX();
    int spacing = 5;
    for(int i=0; i<possibility; i++)
    {
        ImGui::InputFloat(("Probability of possibility " + std::to_string(i+1)).c_str(),&proba[i],.0f,.0f,3);
        lineSize +=  possibilitySize + textSize + spacing;
        if(lineSize +possibilitySize + textSize + spacing < ImGui::GetWindowWidth())
            ImGui::SameLine(.0f,5.f);
        else
            lineSize = ImGui::GetCursorPosX();
    }
    ImGui::PopItemWidth();

    ImGui::Text("Somme : %f",somme);
    ImGui::PopStyleColor();
    ImGui::Separator();
        if(somme -  1.0f < 0.0001f)
    {
        ImGui::PushItemWidth(100);
        ImGui::InputInt("Select possibility",&selectedPossibility);
        ImGui::PopItemWidth();
        if(selectedPossibility > 0 && selectedPossibility <= possibility)
        {
            histogramData.clear();
            if(showHistogram)
            {
                if(ImGui::Button("Hide histogram"))
                {
                    showHistogram = false;
                }
                for(int i =0; i<=repetition; i++)
                {
                    histogramData.push_back(coefBino(repetition,i)*pow(proba[selectedPossibility-1],i) * pow((1-proba[selectedPossibility-1]),(repetition-i)));
                }
                int histoWidth =ImGui::GetWindowWidth() - ImGui::GetCursorPosX()*2;
                ImGui::PlotHistogram("Proba", &(histogramData[0]), histogramData.size(),0,NULL,.0f,1.f,ImVec2(histoWidth,200));
            }
            else
            {
                if(ImGui::Button("Show histogram"))
                {
                    showHistogram = true;
                }
            }
        }
    }
}
ImVec2 nodeSpacing ;
ImVec2 nodeSize = ImVec2(20,20);

int posY;

void addNode(int level,int number,ImVec2 fullTreeSize,int numberNode,int count )
{
    ImGui::SetCursorPosX(nodeSize.x * level + nodeSpacing.x * (level - 1));
    int spacing = fullTreeSize.y / (numberNode+1);
    ImGui::SetCursorPosY(posY + spacing * (number+1));
    const char *name = std::to_string(count).c_str();
    if(count ==0)
        name = (const char*)((std::string)(" ")).c_str();
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
    std::cout<<"2"<<"  "<<count<<std::endl;
    ImGui::SameLine();
    if((level) == repetition+1)
    {
      std::cout<<"1"<<"  "<<count<<std::endl;
        ImGui::Text("%f",totalProba);
        histoData2.push_back(totalProba);
        nameData2.push_back(count);
    }
    ImGui::PopStyleColor();
}

void setupTree()
{
  std::cout<<somme<<std::endl;
      if(somme -  1.0f > 0.0001f)
        return;
    int count = 0;
    std::queue<ImVec2> lastLevel;
    int posX = ImGui::GetCursorPosX();
    posY = ImGui::GetCursorPosY();
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 treeSize;
    ImVec2 originPos;
    ImVec2 endPos;
    ImVec2 fullTreeSize = ImVec2(windowSize.x - posX, windowSize.y - posY);
    int totalNumberNode = pow(possibility,repetition);
    nodeSpacing = ImVec2(((fullTreeSize.x - (nodeSize.x * (repetition+1)))/(repetition+1)),(fullTreeSize.y - (nodeSize.y*(totalNumberNode)))/totalNumberNode);

    float scrollX = ImGui::GetScrollX();
    float scrollY = ImGui::GetScrollY();

    histoData2.clear();
    nameData2.clear();
    for(int i = 0 ; i<= repetition; i++)
    {
        int numberNode = pow(possibility,i);
        for(int j=0; j<numberNode; j++)
        {
            addNode(i+1,j,fullTreeSize,numberNode,count);
            count = addCount(count);
            ImGui::SetCursorPosY(posY);

            endPos = ImVec2(nodeSize.x * (i+1) + nodeSpacing.x * i + windowPos.x,
                            posY + (fullTreeSize.y / (numberNode+1)) * (j+1) + windowPos.y + nodeSize.y/2);
            for(int k=0; k<possibility; k++)
            {
                ImVec2 lastLevelPos = ImVec2(endPos.x +nodeSize.x,endPos.y);
                lastLevel.push(lastLevelPos);
            }
            if(i>0)
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

void addData(int count )
{
    const char *name = std::to_string(count).c_str();
    if(count ==0)
        name = (const char*)((std::string)(" ")).c_str();
    float totalProba = 1;

    for(unsigned int i =0 ; i<strlen(name); i++)
    {
        if(name[i]!=' ')
            totalProba = totalProba * proba[atoi(&name[i])%10-1];
    }
        histoData2.push_back(totalProba);
        nameData2.push_back(count);

}

void setupHisto()
{
  if(somme -  1.0f > 0.0001f)
    return;
    int count = 0;
    std::queue<ImVec2> lastLevel;
    histoData2.clear();
    nameData2.clear();
    for(int i = 0 ; i<= repetition; i++)
    {
        int numberNode = pow(possibility,i);
        for(int j=0; j<numberNode; j++)
        {
            if((i+1) == (repetition+1))
              addData(count);
            count = addCount(count);
        }
    }
}



void ProbaModule::render()
{
    if(!(*openProbaModule))
        return;
    if(!ImGui::Begin("Proba module",openProbaModule,ImGuiWindowFlags_HorizontalScrollbar))
    {
        ImGui::End();
        return;
    }
    if(showField){
      if(ImGui::Button("Hide field")){
        showField = false;
      }
    setupField();
  }else{
    if(ImGui::Button("Show field")){
      showField = true;
    }
  }
    ImGui::Separator();

    if(repetition >= 0 && possibility > 0 )
    {
            if(showHistoTree)
            {
              if(!showTree)
                setupHisto();
                if(ImGui::Button("Hide Histogram of all possibility"))
                {
                    showHistoTree = false;
                }
                if(histoData2.size()>0)
                {
                  if(somme -  1.0f < 0.0001f){

                int histoWidth = ImGui::GetWindowWidth() - ImGui::GetCursorPosX()*2;
                ImGui::customHistogram("Proba of each branch",  &(histoData2[0]), histoData2.size(),&(nameData2[0]), ImVec2(histoWidth,200.f),&selectedNode);
              }else
              std::cout<<somme<<std::endl;
              }
            }
            else
            {
                if(ImGui::Button("Show Histogram of all possibility"))
                {
                    showHistoTree = true;
                }
            }


            if(showTree)
            {
                if(ImGui::Button("Hide tree"))
                {
                    showTree = false;
                }
                  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,50.f);
                setupTree();
                ImGui::PopStyleVar();
            }
            else
            {
                if(ImGui::Button("Show Tree"))
                {
                    showTree = true;
                }
            }

        }

    ImGui::End();
}
