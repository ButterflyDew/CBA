#include "Hub-Labeling/global.h"
#include "Hub-Labeling/hbll.h"
class Core
{
public:
    HBLL hbll;
    double start_time, end_time;
    vector <int> revM;
    Tree Go_Core(Graph G, vector <vector <int> > K, int D);

    Tree FindAns(Graph &G, vector <vector <int> > &K, int D, int v, Tree opt);

    Tree GenAnsEven(Graph &G, vector <vector <int> > &K, int D, int v, vector <int> isu);

    int getpri(vector <vector <int> > &K, int u, int v, int D);

    void Expand(Tree &T, int c, vector <int> path);

    void read_hbll(string filepre);
};