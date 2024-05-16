#pragma once
#include "Hub-Labeling/global.h"
#include "Hub-Labeling/betweenness_centrality.h"
class qHBLL
{
public:
    struct qinfo
    {
        int l, h;
        double d;
        int u;
        qinfo(){}
        qinfo(int L, int H, double D, int U):l(L), h(H), d(D), u(U) {}
    };
    qHBLL(){clear();}
    vector <qinfo> L;
    vector <int> pos;
    vector <int> idx;
    vector <int> lasl;
    vector <int> potl;
    vector <vector <int> > fmi;
    unordered_map <long long, int> fid;
    int typ;
    int rD;
    int cntud, cntwd;
    long long sumud, sumwd;
    void clear();
    void insert_qry(vector <int> &qry, vector < vector <Triple> > &tmp, int D);
    pair <int,double> GET_WD(int h, vector <Triple> &Lv);
    int GET_UD(vector <Triple> &Lv, vector <int> &nxtl);

    pair <int,double> GET_WD_M(int h, vector <Triple> &Lv);
    int GET_UD_M(vector <Triple> &Lv);

    pair <int,int> findseg(int la);
    pair <int,int> findsegbf(int la);
    pair <int,double> GET_WD_L(int h, vector <Triple> &Lv);
    int GET_UD_L(vector <Triple> &Lv, vector <int> &nxtl);
};
