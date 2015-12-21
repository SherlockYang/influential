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
    Analyzer(DataLoader* dataLoader);
    int                 UserInfluence(const char* fileDir);
private:
    DataLoader*         dataLoader;
};
