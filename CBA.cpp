#include "CBA.h"

Tree CBA::Go_CBA(Graph G, vector <vector <int> > K, int D)
{
    hbll.build_hbll(G);

    auto ret = OPT_MC(G, K, D);
    vector <int> M = ret.first;
    int c = ret.second;

    vector <int> S = M;
    sort(S.begin(), S.end());
    S.erase(unique(S.begin(), S.end()), S.end());

    set <int> S_T;
    for(auto v: S) 
        S_T.insert(v);

    vector <int> Vt;
    Vt.push_back(c);
    S_T.erase(c);

    Tree T;
    T.add_root(c);

    while(!S_T.empty())
    {
        int s_ = 0, t_ = 0, mx = inf;
        for(auto s: Vt)
        {
            for(auto t: S_T)
            {
                int len = hbll.GET_WD(s, t, D/2 - T.GET_UD_T(c, s));
                if(len < mx)
                {
                    len = mx;
                    s_ = s, t_ = t;
                }
            }
        }
        vector<int> path = hbll.GET_MWP(s_, t_, D/2 - T.GET_UD_T(c, s_));
        T = Expand(T, c, path);
    }
    return T;
}


Tree Expand(Tree T, int c, vector <int> path)
{
    //if(T.vex.empty()) T.rt = c;
    int x = path[0];
    for(int i = 1; i < path.size(); i++)
    {
        int y = path[i];
        if(T.vex.find(x) != T.vex.end() && T.vex.find(y) != T.vex.end())
        {
            int y_ = T.par[T.Id[y]];
            T.chi[T.Id[y_]].erase(y);
        }
        T.add_edge(x, y);
        x = y;
    }
    return T;
}

//姑且认为 K 是点集的集合
pair <vector <int>, int> CBA::OPT_MC(Graph &G, vector <vector <int> > &K, int D)
{
    int n = G.n;
    vector <int> vis(n+1);
    queue <pair <int, int> > Q;

    set <int> inter;
    for(auto Ki: K) for(auto k: Ki)
        inter.insert(k);

    for(auto u: inter)
    {
        Q.push(make_pair(u, 0));
        vis[u] = 1;
    }

    vector <int> M;
    int c;

    while(!Q.empty())
    {
        auto [v, h] = Q.front();
        vector <int> Mv = MaxM(G, K, D, v);
        if(Mv.size() > M.size() || (Mv.size() == M.size() && get_sum(G, M, v, D) < get_sum(G, M, c, D)))
        {
            M = Mv;
            c = v;
        }
        if(h < D/2)
        {
            for(auto [nxt, w]: G.ed[v])
            {
                if(vis[nxt] == 0)
                {
                    Q.push(make_pair(nxt, h + 1));
                    vis[nxt] = 1;
                }
            }
        }
    }

    return make_pair(M, c);
}

int CBA::get_sum(Graph &G, vector <int> &M, int v, int D)
{
    int sum = 0;
    for(auto u: M)
    {
        sum += hbll.GET_WD(u, v, D/2);
        if(sum > inf) sum = inf;
    }
    return sum;
}

vector <int> CBA::MaxM(Graph &G, vector <vector <int> > &K, int D, int v)
{
    vector <int> M;
    for(auto Ki: K)
    {
        int u, mi = inf;
        for(auto k: Ki)
        {
            int wd = hbll.GET_WD(v, k, D/2);
            if(wd < mi)
            {
                mi = wd;
                u = k;
            }
        }
        if(hbll.GET_WD(v, u, D/2) < inf)
            M.push_back(u);
    }
    return M;
}