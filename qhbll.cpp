#include "qhbll.h"
#include "Hub-Labeling/global.h"
#include <utility>

void qHBLL::clear()
{
    L.clear();// group merge label
    pos.clear();// different l
    fid.clear();// umap -> D * merge label
    cntud = cntwd = 0;
    sumud = sumwd = 0;
    for(auto resl: lasl)
    {
        idx[resl] = -1;
        potl[resl] = -1;
        for(int i = 0; i < rD; i++) 
            if(fmi.size() > i && fmi[i].size() > resl)
                fmi[i][resl] = -1;
    }

    lasl.clear();
    // idx.clear();// max l -> n
    // fmi.clear();// D * max l -> D * n 
    typ = 0;
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
    if(typ == 0)
    {
        idx.resize(L.back().l + 1, -1);
        potl.resize(L.back().l + 1, -1);
    }    
    else
    {
        while(idx.size() < L.back().l + 1) 
            idx.push_back(-1);
        while(potl.size() < L.back().l + 1)
            potl.push_back(-1);
    }
    
    L = tmpL;
    int j = 0;
    for(auto it: L)
    {
        if(pos.empty()) 
        {
            idx[it.l] = pos.size();
            lasl.push_back(it.l);
            pos.push_back(j);
            potl[it.l] = j; 
        }
        else if(L[pos.back()].l != it.l)
        {
            idx[it.l] = pos.size();
            lasl.push_back(it.l);
            pos.push_back(j);
            potl[it.l] = j;
        }
        ++j;
    }
    pos.push_back(L.size());

    //typ = 1;
    rD = D;
    //typ = 1 开表前缀和 typ = 2 hash typ = 0 原双指针
    if(typ == 1)
    {
        //fprintf(stderr, "fsize:%d Lsize%d\n",D*L.back().l + 1, L.size());
        //fmi.resize(D, vector <int> (L.back().l + 1, -1));

        while(fmi.size() < D) fmi.push_back(vector <int> (L.back().l + 1, -1));
        for(int i = 0; i < D; i++)
            while(fmi[i].size() < L.back().l + 1)
                fmi[i].push_back(-1);

        int j = 0;
        for(auto it: L)
            fmi[it.h][it.l] = j++;

        for(int i = 1; i < D; i++)
            for(auto j: lasl)
                if(fmi[i][j] == -1)
                    fmi[i][j] = fmi[i - 1][j];

        // for(int i = 1; i < D; i++)
        //     for(j = 1; j <= L.back().l; j++)
        //     {
        //         if(fmi[i][j] == -1)
        //             fmi[i][j] = fmi[i-1][j];
        //     }
    }
    else if(typ == 2)
    {
        auto getid = [D] (int a, int b){return 1ll * D * b + a;};
        int j = 0;
        for(auto it: L)
        {
            auto pid = getid(it.h, it.l);
            fid[pid] = j;
            for(int i = it.h + 1; i < D; i++)
                fid[getid(i, it.l)] = j;
            ++j;
        }
    }
}

int qHBLL::GET_UD(vector <Triple> &Lv, vector <int> &nxtl)
{
    ++cntud;
    //sumud += Lv.size();
    //return GET_UD_M(Lv);
    return typ == 0? GET_UD_M(Lv) : GET_UD_L(Lv, nxtl);
}   

pair <int, double> qHBLL::GET_WD(int h, vector<Triple> &Lv)
{
    ++cntwd;
    sumwd += Lv.size();
    //return GET_WD_M(h, Lv);
    return typ == 0? GET_WD_M(h, Lv): GET_WD_L(h, Lv);
}

int qHBLL::GET_UD_M(vector <Triple> &Lv)
{
    //fprintf(stderr, "GETUD: [%lu, %lu], ratio = %.5lf\n", L.size(), Lv.size(), 1.0*L.size()/Lv.size());
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

pair <int,int> qHBLL::findsegbf(int la)
{
    int l = inf, r = -inf;
    for(int i = 0; i < L.size(); i++)
        if(L[i].l == la)
        {
            l = min(l, i);
            r = max(r, i);
        }
    if(l == inf) return {-1, -1};
    else return {l,r};
}

pair <int,int> qHBLL::findseg(int la)
{
    int loc;
    if(la >= idx.size() || (loc = idx[la]) == -1) return {-1, -1};
    return {pos[loc], pos[loc + 1] - 1};

    // ignore below
    int l = 0, r = pos.size() - 2;
    // if(r == 0)
    // {
    //     fprintf(stderr, "r == 0\n");
    // }
    if(la < L[pos[l]].l) return {-1, -1};

    while(l < r)
    {
        int mid = (l + r + 1) >> 1;
        int xpos = L[pos[mid]].l;
        if(xpos <= la) 
            l = mid;
        else 
            r = mid - 1; 
    }   
    if(la == L[pos[l]].l) 
    {
        if(l + 1 == pos.size())
            fprintf(stderr, "%d [%d, %d]\n", pos.back(), pos[l], pos[l+1]-1);
        return {pos[l], pos[l+1]-1};
    }
    else return {-1, -1};
}

int qHBLL::GET_UD_L(vector <Triple> &Lv, vector <int> &nxtl)
{
    int ret = inf;
    int j = 0, la, l;
    while(j < Lv.size())
    {
        ++sumud;
        //auto [l,r] = findseg(Lv[j].l);
        la = Lv[j].l;
        l = la < potl.size() && potl[la] != -1 ? potl[la] : -1;
        // l = (la >= idx.size() || idx[la] == -1) ? -1: pos[idx[la]];
        // if(l != -1 && ret > Lv[j].h + L[l].h) ret = Lv[j].h + L[l].h;
        if(l != -1 && ret > Lv[j].h + L[l].h) ret = Lv[j].h + L[l].h;

        j = nxtl[j];
    }

    // if(la >= idx.size() || (loc = idx[la]) == -1) return {-1, -1};
    // return {pos[loc], pos[loc + 1] - 1};

    // for(auto itv: Lv)
    // {
    //     auto [l,r] = findseg(itv.l);
    //     if(l == -1) continue;
    //     ret = min(ret, itv.h + L[l].h);
    // }
    return ret;
}

// l h d u
pair <int, double> qHBLL::GET_WD_M(int h, vector<Triple> &Lv)
{
    //fprintf(stderr, "GETWD: [%lu, %lu], ratio = %.5lf\n", L.size(), Lv.size(), 1.0*L.size()/Lv.size());
    int retu = 0;
    double retw = INF;
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

pair <int,double> qHBLL::GET_WD_L(int h, vector <Triple> &Lv)
{
    int retu = 0;
    double retw = INF;
    for(auto itv: Lv)
    {
        if(typ == 1)
        {
            if(itv.h > h || itv.l > L.back().l) continue;
            int j = fmi[h - itv.h][itv.l];
            if(j != -1 && retw > L[j].d + itv.d)
            {
                retw = L[j].d + itv.d;
                retu = L[j].u;
            }
            continue;
        }
        else if(typ == 2)
        {
            if(itv.h > h || itv.l > L.back().l) continue;
            int D = rD;
            auto getid = [D] (int a, int b){return 1ll * D * b + a;};
            auto pid = getid(h - itv.h, itv.l);
            if(fid.find(pid) != fid.end())
            {
                int j = fid[pid];
                if(j != -1 && retw > L[j].d + itv.d)
                {
                    retw = L[j].d + itv.d;
                    retu = L[j].u;
                }
            }
            continue;
        }
        auto [l,r] = findseg(itv.l);
        if(l == -1) continue;
        // itv.h + L[loc].h <= h
        if(itv.h + L[l].h > h) continue;
        if(l>r)
        {
            fprintf(stderr,"l = %d > r = %d\n", l, r);
            exit(0);
        }
        // for(int i = l; i <= r; i++)
        //     if(retw > L[i].d + itv.d && itv.h + L[i].h <= h)
        //     {
        //         retw = L[i].d + itv.d;
        //         retu = L[i].u;
        //     }
        while(l<r)
        {
            int mid = (l + r + 1) >> 1;
            if(itv.h + L[mid].h <= h)
                l = mid;
            else r = mid - 1;
        }
        if(retw > L[l].d + itv.d)
        {
            retw = L[l].d + itv.d;
            retu = L[l].u;
        }
    }
    return {retu, retw};
}