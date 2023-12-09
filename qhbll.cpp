#include "qhbll.h"
#include "Hub-Labeling/global.h"
#include <utility>

void qHBLL::clear()
{
    L.clear();
    pos.clear();
}

void qHBLL::insert_qry(vector <int> &qry, vector < vector <Triple> > &tmp, int D)
{
    for(auto u: qry)
    {
        for(auto it: tmp[u])
        {
            if(it.h < D)
                L.push_back(qinfo(it.l, it.h, it.d, u));
        }
    }
    sort(L.begin(), L.end(), [&](auto x,auto y)
    {
        return x.l == y.l ? (x.h == y.h ? x.d < y.d : x.h < y.h) : x.l < y.l;
    });
    vector <qinfo> tmpL;
    for(auto it: L)
    {
        if(tmpL.empty()) tmpL.push_back(it);
        else
        {
            auto las = tmpL.back();
            if(las.l != it.l) tmpL.push_back(it);
            else
            {
                if(las.h == it.h) continue;
                if(las.d < it.d) continue;
                tmpL.push_back(it);
            }
        }
    }
    L = tmpL;
    int j = 0;
    for(auto it: L)
    {
        if(pos.empty()) pos.push_back(j);
        else if(L[pos.back()].l != it.l) pos.push_back(j);
        ++j;
    }
    pos.push_back(L.size());
}


int qHBLL::GET_UD(vector <Triple> &Lv)
{
    int idu = 0, idv = 0, ret = inf;
    while(idu < L.size() && idv < Lv.size())
    {
        if(L[idu].l == Lv[idv].l)
        {
            ret = min(ret, L[idu].h + Lv[idv].h);
            ++idu, ++idv;
        }
        else if(L[idu].l < Lv[idv].l) ++idu;
        else ++idv;
    }
    return ret;
}
// l h d u
pair <int, int> qHBLL::GET_WD(int h, vector<Triple> &Lv)
{
    int retu = 0, retw = inf;
    int plu = 0, lu = pos[0], pu = pos[1] - 1;

    for(auto itv: Lv)
    {
        while(plu < pos.size() && L[pos[plu]].l < itv.l) ++plu;
        if(plu == pos.size()) break;
        if(lu != pos[plu])
        {
            lu = pos[plu];
            pu = pos[plu + 1] - 1;
        }
        if(L[pos[plu]].l != itv.l) continue;
        while(pu >= lu && L[pu].h + itv.h > h) --pu;
        if(pu >= lu && retw > L[pu].d + itv.d)
        {
            retw = L[pu].d + itv.d;
            retu = L[pu].u;
        } 
    }
    return make_pair(retu, retw);
}