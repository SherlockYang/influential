#pragma once

#include "Data.h"
#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;

class       Analyzer
{
public:
    // diffusion attributes
    int                 forestNum;
    vector<int>         volume;
    vector<int>         depth;
    vector<int>         speed;
    vector<int>         response;
    vector<int>         breath;
    vector<int>         component;
    vector<int>         cascadeStructure;

    Analyzer(DataLoader* dataLoader);
    double              CalcPageRank();
    double              EdgeWeight(int source, int target, int plan);
    bool                IsRole(User* user, int targetRole);
    int                 CascadeStructure(int targetRole);
    int                 ConnectedComponent(Post* post, int targetRole);
    int                 AnalyzeDiffusion(int targetRole);
    int                 Show(int targetRole);
    int                 Depth(Post* post, int targetRole);
    int                 Volume(Post* post, int targetRole);
    int                 Response(Post* post, int sourceTime, int targetRole);
    int                 Breath(Post* post, int targetRole);
    double              CalcNetworkConstraint(double p);
private:
    DataLoader*         dataLoader;
    double              constraintBound;
    double              pageRankBound;
};
