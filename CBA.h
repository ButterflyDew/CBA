#include "Hub-Labeling/global.h"
#include "Hub-Labeling/hbll.h"
#include "qhbll.h"
#include <vector>
class CBA
{
public:
    HBLL hbll;
    HBLL dhbll[3];
    int algo_typ;
    vector <qHBLL> qhbll;
    // vector <vector <int> > disq;
    // vector <vector <bool> > vq;
    // vector <double> miq;
    // vector <bool> vmq;
    vector <vector <int> > hop_vtog;
    vector <pair <int,int> > res_vtog;
    vector <double> dis_vtoall;
    //vector <int> res_vtoall;

    double start_time, after_optmc_time, end_time, aver_l;
    
    vector <int> revM;
    double revw;

    struct qnode
    {
        int v, h, priv, prt;
        //double dis;
        qnode(){v = h = priv = prt = 0;}
        qnode(int V, int H, int Priv, int Prt): v(V), h(H), priv(Priv), prt(Prt){}
        //qnode(int V, int H, int Priv, int Prt, double Dis): v(V), h(H), priv(Priv), prt(Prt), dis(Dis) {}
        bool friend operator <(qnode a, qnode b)
        {
            //return a.priv == b.priv? a.dis < b.dis : a.priv < b.priv;
            return a.priv < b.priv;
        }
    };
    // struct AddiInfo
    // {
    //     int sum, mi;
    //     AddiInfo(){sum = 0, mi = inf;}
    //     AddiInfo(int Sum,int Mi): sum(Sum), mi(Mi) {}
    // };
    pair <vector <int>, double> Pruned_MaxM(Graph &G, vector <vector <int> > &K, int D, int v);

    Tree Go_CBA(Graph G, vector <vector <int> > K, int D, int typ);

    pair <vector <int>, int> OPT_MC(Graph &G, vector <vector <int> > &K, int D);

    int getpri(vector <vector <int> > &K, int v, int h, int D, int prt);

    pair <vector <int>, int> Pruned_OPT_MC(Graph &G, vector <vector <int> > &K, int D);

    vector <int> MaxM(Graph &G, vector <vector <int> > &K, int D, int v);

    void Expand(Tree &T, int c, vector <int> path);

    double get_sum(Graph &G, vector <int> &M, int v,int D);

    void read_hbll(string filepre);

    void build_qhbll(vector <vector <int> > &K, int D, int typ);
};