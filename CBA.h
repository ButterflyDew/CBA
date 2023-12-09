#include "Hub-Labeling/global.h"
#include "Hub-Labeling/hbll.h"
#include "qhbll.h"
#include <vector>
class CBA
{
public:
    HBLL hbll;
    vector <qHBLL> qhbll;
    vector <vector <int> > disq;
    vector <vector <bool> > vq;
    vector <int> miq;
    vector <bool> vmq;
    double start_time, after_optmc_time, end_time, aver_l;
    
    vector <int> revM;
    double revw;

    struct qnode
    {
        int v, h, priv;
        qnode(){v = h = priv = 0;}
        qnode(int V, int H, int Priv): v(V), h(H), priv(Priv) {}
        bool friend operator <(qnode a, qnode b)
        {
            return a.priv < b.priv;
        }
    };
    // struct AddiInfo
    // {
    //     int sum, mi;
    //     AddiInfo(){sum = 0, mi = inf;}
    //     AddiInfo(int Sum,int Mi): sum(Sum), mi(Mi) {}
    // };
    pair <vector <int>, int> Pruned_MaxM(Graph &G, vector <vector <int> > &K, int D, int v);

    Tree Go_CBA(Graph G, vector <vector <int> > K, int D);

    pair <vector <int>, int> OPT_MC(Graph &G, vector <vector <int> > &K, int D);

    int getpri(vector <vector <int> > &K, int v, int h, int D);

    pair <vector <int>, int> Pruned_OPT_MC(Graph &G, vector <vector <int> > &K, int D);

    vector <int> MaxM(Graph &G, vector <vector <int> > &K, int D, int v);

    void Expand(Tree &T, int c, vector <int> path);

    int get_sum(Graph &G, vector <int> &M, int v,int D);

    void read_hbll(string filepre);

    void build_qhbll(vector <vector <int> > &K, int D);
};