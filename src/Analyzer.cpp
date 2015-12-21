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

int             Analyzer::UserInfluence()
{
    for (unsigned int i = 0; i < dataLoader -> postList.size(); i ++)
    {
        Post* post = dataLoader -> postList[i];
        int uid = post -> user -> id;
        int val = (int) post -> retweetList.size();
    }
}

int             Analyzer::Show(const char* fileDir)
{
    FILE* fout = fopen(fileDir.c_str(), "w");

    for (map<int, int>::iterator it = depthMap.begin(); it != depthMap.end(); it ++)
    {
        //printf("%d : %d %.5lf\n", it -> first, it -> second, (it -> second + 0.0) / forestNum);
        fprintf(fout, "%d : %d %.5lf\n", it -> first, it -> second, (it -> second + 0.0) / forestNum);
    }
    fclose(fout);
    //printf("===== Volume =====\n");
    fout = fopen(("volume_report_" + method + ".txt").c_str(), "w");
    for (map<int, int>::iterator it = volumeMap.begin(); it != volumeMap.end(); it ++)
    {
        //printf("%d : %d %.5lf\n", it -> first, it -> second, (it -> second + 0.0) / forestNum);
        fprintf(fout, "%d : %d %.5lf\n", it -> first, it -> second, (it -> second + 0.0) / forestNum);
    }
    fclose(fout);
    
    //printf("===== Speed =====\n");
    fout = fopen(("speed_report_" + method + ".txt").c_str(), "w");
    for (map<int, int>::iterator it = speedMap.begin(); it != speedMap.end(); it ++)
    {
        //printf("%d : %d %.5lf\n", it -> first, it -> second, (it -> second + 0.0) / forestNum);
        fprintf(fout, "%d : %d %.5lf\n", it -> first, it -> second, (it -> second + 0.0) / forestNum);
    }
    fclose(fout);

    fout = fopen(("response_report_" + method + ".txt").c_str(), "w");
    for (map<int, int>::iterator it = responseMap.begin(); it != responseMap.end(); it ++)
    {
        fprintf(fout, "%d : %d %.5lf\n", it -> first, it -> second, (it -> second + 0.0) / forestNum);
    }
    fclose(fout);

    fout = fopen(("breath_report_" + method + ".txt").c_str(), "w");
    for (map<int, int>::iterator it = breathMap.begin(); it != breathMap.end(); it ++)
    {
        fprintf(fout, "%d : %d %.5lf\n", it -> first, it -> second, (it -> second + 0.0) / forestNum);
    }
    fclose(fout);

    fout = fopen(("component_report_" + method + ".txt").c_str(), "w");
    for (map<int, int>::iterator it = componentMap.begin(); it != componentMap.end(); it ++)
    {
        fprintf(fout, "%d : %d %.5lf\n", it -> first, it -> second, (it -> second + 0.0) / forestNum);
    }
    fclose(fout);

    fout = fopen(("cascade_report_" + method + ".txt").c_str(), "w");
    int sum = 0;
    for (unsigned int i = 0; i < cascadeStructure.size(); i ++)
        sum += cascadeStructure[i];
    for (unsigned int i = 0; i < cascadeStructure.size(); i ++)
        fprintf(fout, "%d : %d %.5lf\n", i, cascadeStructure[i], (cascadeStructure[i] + 0.0) / sum);
    fclose(fout);
    return 0;
}

bool            Analyzer::IsRole(User* user, int targetRole)
{
    if (targetRole == 0)
        return false;
    if (targetRole == 1)
        return (constraintBound > 0 && user -> constraint > 0 && user -> constraint <= constraintBound && user -> pageRank >= pageRankBound / 3);
    return user -> pageRank >= pageRankBound;
}

int             Analyzer::Depth(Post* post, int targetRole)
{
    if (IsRole(post -> user, targetRole))
        return 0;
    /*
    if (targetRole <= 1 && post -> user -> constraint > 0 && constraintBound > 0 && post -> user -> constraint <= constraintBound)
        return 0;
    if (targetRole >= 2 && post -> user -> pageRank >= pageRankBound)
        return 0;
    */
    int res = 1;
    for (unsigned int i = 0; i < post -> retweetList.size(); i ++)
        res = max(res, Depth(post -> retweetList[i], targetRole) + 1);
    return res;
}

int             Analyzer::Volume(Post* post, int targetRole)
{
    if (IsRole(post -> user, targetRole))
        return 0;
    int res = 1;
    for (unsigned int i = 0; i < post -> retweetList.size(); i ++)
    {
        res += Volume(post -> retweetList[i], targetRole);
    }
    return res;
}

int             Analyzer::Response(Post* post, int sourceTime, int targetRole)
{ 
    if (IsRole(post -> user, targetRole))
        return 0;
    int res = post -> postTime - sourceTime;
    for (unsigned int i = 0; i < post -> retweetList.size(); i ++)
    {
        res += Response(post -> retweetList[i], sourceTime, targetRole);
    }
    return res;
}

int             Analyzer::ConnectedComponent(Post* post, int targetRole)
{
    vector<int> postList;
    postList.push_back(post -> id);
    for (unsigned int header = 0; header < postList.size(); header ++)
    {
        Post* currentPost = dataLoader -> postList[postList[header]];
        for (unsigned int i = 0; i < currentPost -> retweetList.size(); i ++)
            postList.push_back(currentPost -> retweetList[i] -> id);
    }
    set<int> allUserSet;
    for (unsigned int i = 0; i < postList.size(); i ++)
    {
        int uid = dataLoader -> postList[postList[i]] -> user -> id;
        if (allUserSet.count(uid) == 0)
            allUserSet.insert(uid);
    }

    postList.clear();
    postList.push_back(post -> id);
    for (unsigned int header = 0; header < postList.size(); header ++)
    {
        Post* currentPost = dataLoader -> postList[postList[header]];
        if (IsRole(currentPost -> user, targetRole))
            continue;
        for (unsigned int i = 0; i < currentPost -> retweetList.size(); i ++)
            postList.push_back(currentPost -> retweetList[i] -> id);
    }
    set<int> userSet;
    for (unsigned int i = 0; i < postList.size(); i ++)
    {
        int uid = dataLoader -> postList[postList[i]] -> user -> id;
        if (userSet.count(uid) == 0)
            userSet.insert(uid);
    }
    postList.clear();
    set<int> scannedUser;
    vector<int> userList;
    int res = 0;
    for (set<int>::iterator it = userSet.begin(); it != userSet.end(); it ++)
    {
        User* user = dataLoader -> userList[*it];
        if (scannedUser.count(user -> id) > 0)
            continue;
        res ++; 
        userList.clear();
        userList.push_back(user -> id);
        scannedUser.insert(user -> id);
        for (unsigned int header = 0; header < userList.size(); header ++)
        {
            User* currentUser = dataLoader -> userList[userList[header]];
            for (unsigned int i = 0; i < currentUser -> followerList.size(); i ++)
            {
                int uid = currentUser -> followerList[i] -> id;
                if (allUserSet.count(uid) == 0 || scannedUser.count(uid) > 0)
                    continue;
                scannedUser.insert(uid);
                userList.push_back(uid);
            }
        }
    }
    return res;
}

int             Analyzer::Breath(Post* post, int targetRole)
{
    vector<int> bfsList;
    vector<int> depthCount;
    bfsList.push_back(post -> id);
    depthCount.push_back(0);
    for (unsigned int head = 0; head < bfsList.size(); head ++)
    {
        Post* currentPost = dataLoader -> postList[bfsList[head]];
        if (IsRole(post -> user, targetRole))
            continue;
        for (unsigned int i = 0; i < currentPost -> retweetList.size(); i ++)
        {
            bfsList.push_back(currentPost -> retweetList[i] -> id);
            depthCount.push_back(depthCount[head] + 1);
        }
    }
    int res = 0;
    for (unsigned int i = 0; i < depthCount.size(); )
    {
        unsigned int j = i + 1;
        while (j < depthCount.size() && depthCount[j] == depthCount[i])
            j ++;
        //if (j - i > res)
        //    res = j - i;
        res += j - i;
        i = j;
    }
    if (depthCount[depthCount.size() - 1] == 0)
        return res;
    return res / (depthCount[depthCount.size() - 1]) + 1;
}

int             Analyzer::CascadeStructure(Post* post)
{
    if (post -> retweetList.size() == 0)
        return 0;
    if (post -> retweetList.size() == 1)
    {
        if (post -> retweetList[0] -> retweetList.size() == 0)
            return 1;
        if (post -> retweetList[0] -> retweetList.size() == 1)
        {
            //if (post -> retweetList[0] -> retweetList[0] -> retweetList.size() == 0)
                return 2;
        }
    }
    int sum = 0;
    for (unsigned int i = 0; i < post -> retweetList.size(); i ++)
        sum += post -> retweetList[i] -> retweetList.size();
    if (sum == 0)
    {
        if (post -> retweetList.size() < 5)
            return post -> retweetList.size() + 1;
    }
    if (post -> retweetList.size() == 2 && sum == 1)
        return 6;
    return 7;
}

int             Analyzer::CascadeStructure(int targetRole)
{
    cascadeStructure.clear();
    for (unsigned int i = 0; i < dataLoader -> postList.size(); i ++)
    {
        Post* post = dataLoader -> postList[i];
        if ((! IsRole(post -> user, targetRole)) && targetRole != 0)
            continue;
        int cascadeIdx = CascadeStructure(post);
        for (unsigned int j = cascadeStructure.size(); j <= cascadeIdx; j ++)
            cascadeStructure.push_back(0);
        cascadeStructure[cascadeIdx] ++;
    }
    return 0;
}

int             Analyzer::AnalyzeDiffusion(int targetRole)
{
    printf("Start to analyze diffusion process...\n");
    printf("#users: %d\n", dataLoader -> userList.size());
    vector<double> constraintList;
    vector<double> pageRankList;
    for (unsigned int i = 0; i < dataLoader -> userList.size(); i ++)
    {
        User* user = dataLoader -> userList[i];
        if (user -> constraint > 1e-10)
            constraintList.push_back(user -> constraint);
        if (user -> pageRank > 0)
            pageRankList.push_back(-1 * user -> pageRank);
    }
    //printf("%d\n", constraintList.size());
    //printf("%d\n", pageRankList.size());
    sort(constraintList.begin(), constraintList.end());
    sort(pageRankList.begin(), pageRankList.end());
    if (constraintList.size() > 0)
        constraintBound = constraintList[constraintList.size() * 0.02];
    pageRankBound = -1 * pageRankList[pageRankList.size() * 0.2];
    printf("constraint bound: %.5lf\n", constraintBound);
    printf("pagerank bound: %.5lf\n", pageRankBound);
    printf("%.3lf%% users been selected as opinion leaders.\n", pageRankList.size() * 20.0 / dataLoader -> userList.size());
    printf("%.3lf%% users been selected as structural hole spanners.\n", constraintList.size() * 2 / dataLoader -> userList.size());

    // random selection
    vector<pair<int, double> > randomPairList;
    if (targetRole == 3)
    {
        set<int> selectSet;
        for (unsigned int i = 0; i < dataLoader -> userList.size(); i ++)
            dataLoader -> userList[i] -> pageRank = 0;
        for (unsigned int i = 0; i < pageRankList.size() * 0.1; i ++)
        {
            while (true)
            {
                int idx = rand() % dataLoader -> userList.size();
                if (selectSet.count(idx) == 0)
                {
                    selectSet.insert(idx);
                    randomPairList.push_back(make_pair(idx, dataLoader -> userList[idx] -> pageRank));
                    dataLoader -> userList[idx] -> pageRank = 1;
                    break;
                }
            }
        }
        pageRankBound = 1;
    }
    forestNum = 0;
    volume.clear();
    depth.clear();
    response.clear();
    speed.clear();
    breath.clear();
    component.clear();
    for (unsigned int i = 0; i < dataLoader -> postList.size(); i ++)
    {
        Post* post = dataLoader -> postList[i];
        if (post -> sourcePost -> id == post -> id)
        {
            forestNum ++;
            depth.push_back(Depth(post, targetRole));
            int total = Volume(post, targetRole);
            volume.push_back(total); 
            int responseTime = Response(post, post -> postTime, targetRole);
            if (total > 0)
                response.push_back((responseTime + 0.0) / total);
            else
                response.push_back(0);
            breath.push_back(Breath(post, targetRole));
            component.push_back(ConnectedComponent(post, targetRole));
            int s = -1;
            for (unsigned int j = 0; j < post -> retweetList.size(); j ++)
            {
                if (IsRole(post -> retweetList[j] -> user, targetRole))
                    continue;
                if (s == -1 || post -> retweetList[j] -> postTime < s)
                    s = post -> retweetList[j] -> postTime;
            }
            if (s >= 0)
                speed.push_back(s - post -> postTime);
            else
                speed.push_back(0);
        }
    }
    CascadeStructure(targetRole);

    // recover pagerank for random selection
    for (unsigned int i = 0; i < randomPairList.size(); i ++)
    {
        dataLoader -> userList[randomPairList[i].first] -> pageRank = randomPairList[i].second;
    }
    return 0;
}

double          Analyzer::EdgeWeight(int source, int target, int plan)
{
    double res;
    if (plan == 0)
        res = dataLoader -> userList[source] -> followeeList.size();
    if (plan == 1)
        res = dataLoader -> userList[target] -> followerList.size();
    if (plan == 2)
        res = sqrt(dataLoader -> userList[source] -> followeeList.size() * dataLoader -> userList[target] -> followerList.size());
    if (res > 0)
        return 1.0 / res;
    return 0;
}

bool            edgeCmp(pair<int, int> a, pair<int, int> b)
{
    if (a.first != b.first)
        return a.first < b.first;
    return a.second < b.second;
}

double          Analyzer::CalcPageRank()
{
    FILE* fout = fopen("pagerank.txt", "w");
    for (unsigned int i = 0; i < dataLoader -> userList.size(); i ++)
    {
        User* user = dataLoader -> userList[i];
        user -> pageRank = (int) user -> followerList.size();
        fprintf(fout, "%s %.5lf\n", user -> name.c_str(), user -> pageRank);
    }
    fclose(fout);
    return 0;
}

double          Analyzer::CalcNetworkConstraint(double p)
{
    set<pair<int, int> > edgeSet;
    vector<pair<int, int> > edgeList;
    for (unsigned int i = 0; i < dataLoader -> userList.size(); i ++)
    {
        User* user = dataLoader -> userList[i];
        for (unsigned int j = 0; j < user -> followeeList.size(); j ++)
        {
            int a = user -> id;
            int b = user -> followeeList[j] -> id;
            pair<int, int> key = make_pair(a, b);
            if (edgeSet.find(key) != edgeSet.end())
                continue;
            edgeSet.insert(key);
            edgeList.push_back(key);
        }
    }
    sort(edgeList.begin(), edgeList.end(), edgeCmp);
    vector<double> valList;
    int last = -1;
    for (unsigned int start = 0; start < edgeList.size(); )
    {
        int proc = start * 100 / edgeList.size();
        if (proc > last)
        {
            last = proc;
            printf("Processing %%%d...\n", proc);
        }
        unsigned int end = start + 1;
        while (end < edgeList.size() && edgeList[start].first == edgeList[end].first)
            end ++;
        int d_i = (int) dataLoader -> userList[edgeList[start].first] -> followeeList.size(); 
        double val = 0.0;
        for (unsigned int i = start; i < end; i ++)
        {
            double tmp = 0.0;
            for (unsigned int j = start; j < end; j ++)
            {
                if (i == j)
                    continue;
                pair<int, int> key = make_pair(edgeList[i].second, edgeList[j].second);
                if (edgeSet.find(key) != edgeSet.end())
                {
                    int d_k = (int) dataLoader -> userList[edgeList[j].second] -> followeeList.size();    
                    if (d_i > 0 && d_k > 0)
                    {
                        tmp += 1.0 / (d_i + 0.0) * 1.0 / (d_k + 0.0);
                    }
                }
            }
            if (d_i > 0)
                tmp += 1.0 / (d_i + 0.0);
            val += pow(tmp, 2);
        }
        dataLoader -> userList[edgeList[start].first] -> constraint = val;
        if (val > 0)
            valList.push_back(val);
        start = end;
    }
    FILE* fout = fopen("constraint.txt", "w");
    for (unsigned int i = 0; i < dataLoader -> userList.size(); i ++)
    {
        User* user = dataLoader -> userList[i];
        fprintf(fout, "%s %.5lf\n", user -> name.c_str(), user -> constraint);
    }
    fclose(fout);
    sort(valList.begin(), valList.end());
    edgeList.clear();
    int idx = valList.size() * p;
    if (idx >= valList.size())
        idx = (int) valList.size() - 1;
    constraintBound = valList[idx];
    return valList[idx];
}

int             Analyzer::CalcNetworkConstraint(int plan)
{
    set<pair<int, int> > edgeSet;
    for (unsigned int i = 0; i < dataLoader -> userList.size(); i ++)
    {
        User* user = dataLoader -> userList[i];
        for (unsigned int j = 0; j < user -> followeeList.size(); j ++)
        {
            int a = user -> id;
            int b = user -> followeeList[j] -> id;
            pair<int, int> key = make_pair(a, b);
            if (edgeSet.find(key) != edgeSet.end())
                continue;
            edgeSet.insert(key);
        }
    }
    vector<double> shScoreList;
    int proc = -1;
    for (unsigned int i = 0; i < dataLoader -> userList.size(); i ++)
        shScoreList.push_back(0.0);
    for (unsigned int i = 0; i < dataLoader -> userList.size(); i ++)
    {
        if ((int) (i * 100 / dataLoader -> userList.size()) > proc)
        {
            proc = i * 100 / dataLoader -> userList.size();
            printf("Processing %d%%...\n", proc);
        }
        User* B = dataLoader -> userList[i];
        for (unsigned int j = 0; j < B -> followeeList.size(); j ++)
        {
            User* C = B -> followeeList[j];
            shScoreList[C -> id] += EdgeWeight(B -> id, C -> id, plan);
            //printf("%s -> %s: %.5lf\n", B -> name.c_str(), C -> name.c_str(), EdgeWeight(B -> id, C -> id, plan));
            for (unsigned int k = 0; k < C -> followeeList.size(); k ++)
            {
                User* A = C -> followeeList[k];
                if (A -> id == B -> id)
                    continue;
                pair<int, int> key = make_pair(B -> id, A -> id);
                if (edgeSet.find(key) == edgeSet.end())
                    continue;
                int id = A -> id;
                double val = EdgeWeight(B -> id, C -> id, plan) * EdgeWeight(C -> id, A -> id, plan);
                //printf("%s %s %s: %.5lf\n", A -> name.c_str(), B -> name.c_str(), C -> name.c_str(), val);
                shScoreList[id] += val;         
            }
        }
    }
    for (unsigned int i = 0; i < dataLoader -> userList.size(); i ++)
    {
        //printf("%.5lf\n", shScoreList[i]);
        if (dataLoader -> userList[i] -> followerList.size() > 0)
            shScoreList[i] = pow(shScoreList[i], 2) / dataLoader -> userList[i] -> followerList.size();
        else
            shScoreList[i] = 1.0;
    }

    FILE* fout = fopen(("sh" + Util::Int2Str(plan) + ".txt").c_str(), "w");
    for (unsigned int i = 0; i < dataLoader -> userList.size(); i ++)
    {
        fprintf(fout, "%s %.8lf\n", dataLoader -> userList[i] -> name.c_str(), shScoreList[i]);
    }
    fclose(fout);
    return 0;
}

bool                shCmp(pair<string, double> a, pair<string, double> b)
{
    return a.second < b.second;
}

/*
int                 Analyzer::LoadSH(const string& fileDir)
{
    vector<pair<string, double> > shList;
    vector<string> inputs = Util::ReadFromFile(fileDir.c_str());
    for (unsigned int i = 0; i < inputs.size(); i ++)
    {
        vector<string> tmp = Util::StringTokenize(inputs[i]);
        shList.push_back(make_pair(tmp[0], Util::String2Double(tmp[1])));
    }
    printf("#nodes: %d\n", (int) shList.size());
    sort(shList.begin(), shList.end(), shCmp);
    FILE* fout = fopen("sorted_nc_0.txt", "w");
    for (unsigned int i = 0; i < shList.size(); i ++)
    {
        fprintf(fout, "%s %.10lf\n", shList[i].first.c_str(), shList[i].second);
    }
    fclose(fout);
    return 0;
}
*/

/*
int                 Analyzer::ActiveNeighbor(int featureId, int pos)
{
    vector<double> features;
    for (unsigned int i = 0; i < nodeList.size(); i ++)
    {
        //printf("%s : %.5lf\n", nodeList[i] -> name.c_str(), nodeList[i] -> featureList[featureId]);
        features.push_back(-1 * pos * nodeList[i] -> featureList[featureId]);
    }
    sort(features.begin(), features.end());
    double T = features[features.size() * 0.1];
    T *= -1 * pos;
    printf("threshold: %.5lf\n", T);
    map<pair<int, int>, int> inactiveMap;
    vector<int> active;
    inactiveMap.clear();
    active.clear();
    for (unsigned int i = 0; i < postList.size(); i ++)
    {
        Post* post = postList[i];
        User* node = post -> user;
        int total = 0;
        for (unsigned int j = 0; j < post -> influencedBy.size(); j ++)
        {
            Node* source = post -> influencedBy[j] -> user;
            if (source -> featureList[featureId] * pos >= T * pos)
                total ++;
        }
        for (unsigned int j = active.size(); (int) j <= total; j ++)
            active.push_back(0);
        active[total] ++;
        for (unsigned int j = 0; j < node -> inEdgeList.size(); j ++)
        {
            Node* target = node -> inEdgeList[j];
            pair<int, int> key = make_pair(post -> sourcePost -> id, target -> id);
            if (target -> GetPostIdBySource(post -> sourcePost -> id) == -1)
            {
                map<pair<int, int>, int>::iterator it = inactiveMap.find(key);
                if (it != inactiveMap.end())
                    it -> second ++;
                else
                    inactiveMap.insert(make_pair(key, 1));
            }
        }
    }
    vector<int> inactive;
    inactive.clear();
    for (map<pair<int, int>, int>::iterator it = inactiveMap.begin(); it != inactiveMap.end(); it++)
    {
        int idx = it -> second;
        for (unsigned int i = inactive.size(); (int) i <= idx; i ++)
            inactive.push_back(0);
        inactive[idx] ++;
    }
    FILE* fout = fopen(("analysis_" + Util::Int2Str(featureId) + ".txt").c_str(), "w");
    for (unsigned int i = 0; i < active.size() && i < inactive.size(); i ++)
    {
        if (active[i] > 0 || inactive[i] > 0)
        fprintf(fout, "%d %d %d\n", (int) i, active[i], inactive[i]);
    }
    fclose(fout);
    return 0;
}
*/
