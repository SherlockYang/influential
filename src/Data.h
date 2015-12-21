#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
#include <cmath>

using namespace std;

class       User
{
public:
    User(int id, string name);
    int             id;
    string          name;
    vector<User*>   followerList;
    vector<User*>   followeeList;
    vector<double>  influenceList;
    int             AddInfluence(double val, int t);
    int             CountInfluence();
private:
    vector<int>     countList;
};

class       Paper 
{
public:
    Paper();
    int             id;
    string          key;
    int             publishTime;
    double          mu;
    double          delta;
    vector<Paper*>  referList;
    vector<Paper*>  citationList;
    vector<User*>   authorList;
};

class       Post
{
public:
    int             id;
    int             postTime;
    int             contentLength;
    string          name;
    string          content;
    string          source;
    User*           user;
    Post*           repostFrom;
    Post*           sourcePost;
    vector<Post*>   retweetList;
    Post(int id, string name);
};

class   DataLoader
{
public:
    int                 TIME_STEP;
    vector<string>      newInputs;
    vector<User*>       userList;
    vector<Paper*>      paperList;
    vector<Post*>       postList;
    vector<int>         sourceList;
    set<string>         activeUserSet;
    map<string, int>    paperMap;
    map<int, int>       sourceMap;
    map<string, int>    userIdMap;
    map<string, int>    postIdMap;
    
    DataLoader();
    int                 LoadDegree(string fileDir);
    int                 LoadFlickr(string fileDir);
    int                 LoadPaper(string citationFile, string paperFile, string authorFile);
    int                 LoadData(string networkFile, string postFile);
    int                 LoadNetwork(string fileDir);
    int                 LoadContent(string fileDir);
    int                 LoadDiffusion(string fileDir);
    int                 GetPaperId(const string key);
    int                 GetSourceId(const int key);
    int                 GetUserId(const string& key);
    int                 GetPostId(const string& key);
    int                 GetOrInsertPaperId(const string key);
    int                 GetOrInsertSourceId(const int key);
    int                 GetOrInsertUserId(const string& key);
    int                 GetOrInsertPostId(const string& key);
};
