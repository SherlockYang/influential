#include "Util.h"
#include "Data.h"
#include "Analyzer.h"

#include <sstream>
#include <fstream>
#include <cstdio>

using namespace std;

int     main()
{
    int TIME_STEP = 600; // m: 60, h: 3600
    string NETWORK_FILE_DIR = "/mnt/hd2/Tencent_Follow_data/Tencent_Follow_data/";
    string DIFFUSION_FILE_DIR = "/home/yang/data/tencent_weibo/hot_posts100.txt";
    string PAPER_FILE_DIR = "../../../data/citation/paper.txt";
    string CITATION_FILE_DIR = "/home/yang/data/citation/citation.txt";
    string AUTHOR_FILE_DIR = "/home/yang/data/citation/author2paper.txt";

    DataLoader* dataLoader = new DataLoader();
    dataLoader -> TIME_STEP = TIME_STEP;
    //dataLoader -> LoadPaper(CITATION_FILE_DIR, PAPER_FILE_DIR, AUTHOR_FILE_DIR);
    //dataLoader -> LoadFlickr(FLICKR_FILE_DIR);
    //PaperMacroAnalaysis(dataLoader -> paperList);
    dataLoader -> LoadData(NETWORK_FILE_DIR, DIFFUSION_FILE_DIR);
    //dataLoader -> LoadDiffusion(DIFFUSION_FILE_DIR);

    Analyzer* analyzer = new Analyzer(dataLoader);
    analyzer -> UserInfluence("./output/user_influence.out");
    return 0;
}

