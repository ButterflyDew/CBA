#include "Hub-Labeling/global.h"
#include "Hub-Labeling/hbll.h"
class CBA
{
public:
    HBLL hbll;
    Tree Go_CBA(Graph G, vector <vector <int> > K, int D);

    pair <vector <int>, int> OPT_MC(Graph &G, vector <vector <int> > &K, int D);

    vector <int> MaxM(Graph &G, vector <vector <int> > &K, int D, int v);

    void Expand(Tree &T, int c, vector <int> path);

    int get_sum(Graph &G, vector <int> &M, int v,int D);

    void read_hbll(string filepre);
};