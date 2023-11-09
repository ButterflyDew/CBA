#include "CBA.h"
#include "Hub-Labeling/global.h"
#include <array>
#include <cstdio>
#include <queue>
#include <string>


void CBA::Expand(Tree &T, int c, vector <int> path)
{
    //if(T.vex.empty()) T.rt = c;
    // fprintf(stderr, "In Expand: Initial T is:\n");
    // T.Print();

    int x = path[0];
    for(int i = 1; i < path.size(); i++)
    {
        int y = path[i];
        //cerr << "y " << y << endl;
        if(T.vex.find(x) != T.vex.end() && T.vex.find(y) != T.vex.end())
        {
            //fprintf(stderr, "[%d %d] is in T\n", x,y);
            int y_ = T.par[T.Id[y]];
            T.chi[y_].erase(T.Id[y]);
        }
        //cerr << "before add edge" << endl;
        T.add_edge(x, y);
        x = y;
        // fprintf(stderr, "In Expand: after %d add T is:\n", i);
        // T.Print();
    }
}

//姑且认为 K 是点集的集合, O(n kglog)
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
        Q.pop();

        //fprintf(stderr, "In OPT_MC, Q's front = [%d %d]\n", v, h);
        
        vector <int> Mv = MaxM(G, K, D, v);
        if(Mv.size() > M.size() || (Mv.size() == M.size() && get_sum(G, Mv, v, D) < get_sum(G, M, c, D)))
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
//qglog
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
// O(kglog)
vector <int> CBA::MaxM(Graph &G, vector <vector <int> > &K, int D, int v)
{
    //fprintf(stderr, "In MaxM: v is %d\n", v);
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
        if(mi != inf && hbll.GET_WD(v, u, D/2) < inf)
            M.push_back(u);
    }

    // fprintf(stderr, "In MaxM: M is:\n");
    // for(auto x:M) 
    //     fprintf(stderr, "%d ", x);
    // fprintf(stderr, "\n\n");

    return M;
}
//点数上界 qD O((qD)^3glog) +  O(n kglog)
Tree CBA::Go_CBA(Graph G, vector <vector <int> > K, int D)
{
    //hbll.build_hbll(G);
    start_time = get_now_time();

    //auto [M, c] = OPT_MC(G, K, D);
    auto [M, c] = Pruned_OPT_MC(G, K, D);


    after_optmc_time =get_now_time();
    // cerr << "OPT_MC ok" << endl;
    // fprintf(stderr, "Initial M:\n");
    // for(auto x: M) fprintf(stderr, "%d ", x);
    // fprintf(stderr, "\n Initial c: %d\n", c);

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

    //cerr << "Go_CBA: before add path" << endl;

    while(!S_T.empty())
    {
        int s_ = 0, t_ = 0, mi = inf;
        for(auto s: Vt)
        {
            for(auto t: S_T)
            {
                int len = hbll.GET_WD(s, t, D/2 - T.GET_UD_T(c, s));
                //fprintf(stderr, "In add path: [%d %d], length(c,s): %d, len: %d\n", s, t, T.GET_UD_T(c, s), len);
                if(len < mi)
                {
                    mi = len;
                    s_ = s, t_ = t;
                }
            }
        }
        //fprintf(stderr, "[s_, t_] = [%d %d]\n", s_, t_);

        vector<int> path = hbll.GET_MWP(s_, t_, D/2 - T.GET_UD_T(c, s_));
        for(auto x: path)
        {
            Vt.push_back(x);
            S_T.erase(x);
        }    

        // fprintf(stderr, "path is: ");
        // for(auto x: path)
        //     fprintf(stderr, "%d ", x);
        // fprintf(stderr, "\n");
        
        Expand(T, c, path);

        // static int d_ct = 0;
        // ++d_ct;
        // cerr << "the " << d_ct << "add is ok" << endl;
    }
    end_time = get_now_time();
    return T;
}


void CBA::read_hbll(string filepre)
{
    ifstream inputFile(filepre + "/hbll.txt");

    if (!inputFile.is_open()) 
        cerr << "Can't open file: " <<  filepre + "/hbll.txt" << endl;

    string line;
    hbll.L.push_back(vector <HBLL::Triple> ());
    hbll.prew.push_back(vector <int> ());

    while (getline(inputFile, line)) 
    {
        istringstream lines(line);
        string lin;
        ++ hbll.n;
        hbll.L.push_back(vector <HBLL::Triple> ());
        hbll.prew.push_back(vector <int> ());
        while(getline(lines, lin, ','))
        {
            //cout << "set " << lin << endl;
            auto num = extractIntegers(lin);
            if(num.size() == 4)
            {
                hbll.L[hbll.n].push_back(HBLL::Triple(num[0], num[1], num[2]));
                hbll.prew[hbll.n].push_back(num[3]);
            }
            else 
                cerr << "invalid hbll input" << endl;
        }   
    }

    aver_l = hbll.Average_L();
}

int CBA::getpri(vector <vector <int> > &K, int v, int h, int D)
{
    int ret = 0;
    for(auto Ki: K)
    {
        int flg = 0;
        for(auto k: Ki)
            if(h + hbll.GET_UD(k, v) <= D)
            {
                flg = 1;
                break;
            }
        ret += flg;
    }
    return ret;
}

pair <vector <int>, int> CBA::Pruned_OPT_MC(Graph &G, vector <vector <int> > &K, int D)
{
    int n = G.n;
    vector <int> checked(n+1), mhp(n+1, inf);
    priority_queue<qnode> Q;
    
    set <int> inter;
    for(auto Ki: K) for(auto k: Ki)
        inter.insert(k);

    for(auto u: inter)
    {
        Q.push(qnode(u, 0, getpri(K, u, 0, D)));
        mhp[u] = 0;
    }

    vector <int> M;
    int c = 0;

    while(!Q.empty())
    {
        auto qnow = Q.top();
        Q.pop();
        int v = qnow.v, h = qnow.h, priv = qnow.priv;
        int Ms = M.size();
        if(priv < Ms) break;
        if(priv == Ms)
        {
            int mi = inf;
            for(auto Ki: K)
            {
                for(auto k: Ki)
                    mi = min(mi, hbll.GET_WD(v, k, D/2));
            }
            if(mi >= get_sum(G, M, c, D)/Ms)
                continue;
        }
        if(!checked[v])
        {
            vector <int> Mv = MaxM(G, K, D, v);
            checked[v] = true;
            if(Mv.size() > M.size() || (Mv.size() == M.size() && get_sum(G, Mv, v, D) < get_sum(G, M, c, D)))
            {
                M = Mv;
                c = v;
            }
        }
        if(h < D/2)
        {
            for(auto [nxt, w]: G.ed[v])
            {
                if(h + 1 < mhp[nxt])
                {
                    Q.push(qnode(nxt, h + 1, getpri(K, nxt, h + 1, D)));
                    mhp[nxt] = h + 1;
                }
            }
        }
    }
    return make_pair(M, c);
}