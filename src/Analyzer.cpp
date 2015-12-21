#include "Util.h"
#include "Data.h"
#include "Analyzer.h"

#include <algorithm>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cmath>
#include <algorithm>

#define MAXINT 10000000
#define MIN_POST_APPEARENCE 5

using namespace std;

bool    DecreaseCmp(double a, double b)
{
    return a > b;
}

Analyzer::Analyzer(DataLoader* dataLoader)
{
    this -> dataLoader = dataLoader;
}

int             Analyzer::UserInfluence(int topK, const char* fileDir)
{
    for (unsigned int i = 0; i < dataLoader -> postList.size(); i ++)
    {
        Post* post = dataLoader -> postList[i];
        int uid = post -> user -> id;
        int val = (int) post -> retweetList.size();
        dataLoader -> userList[uid] -> AddInfluence(val, post -> postTime);
    }

    // rank user influence
    vector<double> userInfluenceList;
    for (unsigned int i = 0; i < dataLoader -> userList.size(); i ++)
    {
        User* user = dataLoader -> userList[i];
        double totalInfluence = 0.0;
        for (unsigned int j = 0; j < user -> influenceList.size(); j ++)
            totalInfluence += user -> influenceList[j];
        userInfluenceList.push_back(totalInfluence);
    }
    sort(userInfluenceList.begin(), userInfluenceList.end(), DecreaseCmp);
    for (int i = 0; i < topK; i ++)
        printf("%.5lf\n", userInfluenceList[i]);

    // dynamic of overall influence
    vector<double> influenceList;
    influenceList.clear();
    for (unsigned int i = 0; i < dataLoader -> userList.size(); i ++)
    {
        User* user = dataLoader -> userList[i];
        double totalInfluence = 0.0;
        for (unsigned int j = 0; j < user -> influenceList.size(); j ++)
            totalInfluence += user -> influenceList[j];
        if (topK > 0 && totalInfluence < userInfluenceList[topK - 1])
            continue;
        for (unsigned int j = 0; j < user -> influenceList.size(); j ++) 
        {
            for (unsigned int k = influenceList.size(); k <= j; k ++)
                influenceList.push_back(0.0);
            influenceList[j] += user -> influenceList[j];
        }
    }
    for (unsigned int i = 0; i < influenceList.size(); i ++)
    {
        if (topK > 0)
            influenceList[i] /= topK;
        else
            influenceList[i] /= (dataLoader -> userList.size() + 0.0);
    }
    FILE* fout = fopen(fileDir, "w");
    for (unsigned int i = 0; i < influenceList.size(); i ++)
    {
        fprintf(fout, "%d %.5lf\n", i, influenceList[i]);
    }

    /*
    for (unsigned int i = 0; i < dataLoader -> userList.size(); i ++)
    {
        User* user = dataLoader -> userList[i];
        fprintf(fout, "%d", user -> id);
        for (unsigned int j = 0; j < user -> influenceList.size(); j ++)
        {
            fprintf(fout, " %d:%.3lf", j, user -> influenceList[j]);
        }
        fprintf(fout, "\n");
    }
    */
    fclose(fout);
    return 0;
}

