#pragma once
#include "Hub-Labeling/global.h"
#include "Hub-Labeling/betweenness_centrality.h"
class qHBLL
{
public:
    struct qinfo
    {
        int l, h, d, u;
        qinfo(){}
        qinfo(int L, int H, int D, int U):l(L), h(H), d(D), u(U) {}
    };
    qHBLL(){clear();}
    vector <qinfo> L;
    vector <int> pos;
    void clear();
    void insert_qry(vector <int> &qry, vector < vector <Triple> > &tmp, int D);
    pair <int,int> GET_WD(int h, vector <Triple> &Lv);
    int GET_UD(vector <Triple> &Lv);
};

