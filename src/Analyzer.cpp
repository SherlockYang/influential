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

Analyzer::Analyzer(DataLoader* dataLoader)
{
    this -> dataLoader = dataLoader;
}

int             Analyzer::UserInfluence(const char* fileDir)
{
    for (unsigned int i = 0; i < dataLoader -> postList.size(); i ++)
    {
        Post* post = dataLoader -> postList[i];
        int uid = post -> user -> id;
        int val = (int) post -> retweetList.size();
        dataLoader -> userList[uid] -> AddInfluence(val, post -> postTime);
    }

    // dynamic of overall influence
    FILE* fout = fopen(fileDir, "w");
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
    fclose(fout);
    return 0;
}

