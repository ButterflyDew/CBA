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

double CBA::get_sum(Graph &G, vector <int> &M, int v, int D)
{
    double sum = 0;
    for(auto u: M)
    {
        sum += hbll.GET_WD(u, v, D/2);
        if(sum > INF) sum = INF;
    }
    return sum;
}

vector <int> CBA::MaxM(Graph &G, vector <vector <int> > &K, int D, int v)
{
    //fprintf(stderr, "In MaxM: v is %d\n", v);
    vector <int> M;
    for(auto Ki: K)
    {
        int u;
        double mi = INF;
        for(auto k: Ki)
        {
            double wd = hbll.GET_WD(v, k, D/2);
            if(wd < mi)
            {
                mi = wd;
                u = k;
            }
        }
        if(abs(mi - INF) > eps && hbll.GET_WD(v, u, D/2) < INF)
            M.push_back(u);
    }

    // fprintf(stderr, "In MaxM: M is:\n");
    // for(auto x:M) 
    //     fprintf(stderr, "%d ", x);
    // fprintf(stderr, "\n\n");

    return M;
}

void CBA::build_qhbll(vector <vector <int> > &K, int D, int typ)
{
    while(qhbll.size() < K.size() + 1) qhbll.push_back(qHBLL());
    // qhbll.clear();
    // qhbll.resize(K.size() + 1);
    vector <int> fq;
    int j = 0;
    for(auto ki: K)
    {
        qhbll[++j].clear();
        qhbll[j].typ = typ;
        qhbll[j].insert_qry(ki, hbll.L, D);
        for(auto k: ki) fq.push_back(k);
    }
    qhbll[0].clear();
    qhbll[0].typ = typ;
    qhbll[0].insert_qry(fq, hbll.L, D);
}
// typ: 3 CBA 4 CBA+ 5 CBA_list 6 CBA_hash
Tree CBA::Go_CBA(Graph G, vector <vector <int> > K, int D, int typ)
{
    //hbll.build_hbll(G);
    build_qhbll(K, D/2+1, max(0, typ - 4));

    dis_vtoall.resize(G.n + 2, -1);
    if(hop_vtog.empty()) hop_vtog.resize(K.size() + 1, vector <int> (G.n + 1, -1) );
    else
    {
        while(hop_vtog.size()  < K.size() + 1) hop_vtog.emplace_back(vector <int> (G.n + 1, -1));
        for(int i = 0; i < K.size()  + 1; i++)
            while(hop_vtog[i].size() < G.n + 1)
                hop_vtog[i].emplace_back(-1);
    }

    algo_typ = typ - 3;
    start_time = get_now_time();
    
    //auto [M, c] = OPT_MC(G, K, D);
    auto [M, c] = typ == 3 ? OPT_MC(G, K, D): Pruned_OPT_MC(G, K, D);
    revM = M;

    after_optmc_time = get_now_time();
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
        int s_ = 0, t_ = 0;
        double mi = INF;
        for(auto s: Vt)
        {
            for(auto t: S_T)
            {
                double len = hbll.GET_WD(s, t, D/2 - T.GET_UD_T(c, s));
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
    T.gval = G.gval;
    revw = T.get_sumvalue();

    return T;
}


void CBA::read_hbll(string filepre)
{
    fprintf(stderr,"Starting reading HBLL\n");
    ifstream inputFile(filepre);

    if (!inputFile.is_open()) 
        cerr << "Can't open file: " <<  filepre << endl;
    inputFile.tie(nullptr);

    hbll.clear();
    hbll.clearbuc();
    hbll.clearcnt();
    hbll.L.push_back(vector <Triple> ());
    hbll.prew.push_back(vector <int> ());
    for(int D = 2; D <= 6; D += 2)
    {
        int id = D/2-1;
        dhbll[id].clear();
        dhbll[id].clearbuc();
        dhbll[id].clearcnt();
        dhbll[id].L.push_back(vector <Triple> ());
        dhbll[id].prew.push_back(vector <int> ());
    }

    
    string line;
    int per = -1, n, ic;
    while (getline(inputFile, line)) 
    {
        istringstream lines(line);
        if(per == -1)
        {
            per = 1;
            ic = 0;
            lines >> n;    
            continue;
        }
        ++ic;
        if(ic == n/100*per)
        {
            cerr << "Finish input "<< per <<"%\n";
            ++per;
        }
        string lin;
        ++ hbll.n;
        hbll.L.push_back(vector <Triple> ());
        hbll.prew.push_back(vector <int> ());
        for(int D = 2; D <= 6; D += 2)
        {
            int id = D/2-1;
            ++dhbll[id].n;
            dhbll[id].L.push_back(vector <Triple> ());
            dhbll[id].prew.push_back(vector <int> ());
        }
        while(getline(lines, lin, ','))
        {
            //cout << "set " << lin << endl;
            auto [a, b, c, d] = extract_values(lin);
            hbll.L[hbll.n].push_back(Triple(a, b, c));
            hbll.prew[hbll.n].push_back(d);
            for(int D = 2; D <= 6; D += 2)
                if(b <= D/2)
                {
                    int id = D/2-1;
                    dhbll[id].L[dhbll[id].n].push_back(Triple(a, b, c));
                    dhbll[id].prew[dhbll[id].n].push_back(d);
                }
            // auto num = extractIntegers(lin);
            // if(num.size() == 4)
            // {
            //     hbll.L[hbll.n].push_back(Triple(num[0], num[1], num[2]));
            //     hbll.prew[hbll.n].push_back(num[3]);
            // }
            // else 
            //     cerr << "invalid hbll input" << endl;
        }   
    }
    hbll.rearrange();
    aver_l = hbll.Average_L();
    double aver_ld[4]={};
    for(int D = 2; D <= 6; D += 2)
    {
        int id = D/2-1;
        dhbll[id].rearrange();
        dhbll[id].arrange_nxtl();
        aver_ld[id] = dhbll[id].Average_L();
    }
    fprintf(stderr, "averl: %lf %lf %lf %lf\n",aver_l, aver_ld[0], aver_ld[1], aver_ld[2]);
}

int CBA::getpri(vector <vector <int> > &K, int v, int h, int D, int prt)
{
    int ret = 0, curid = -1;
    for(auto Ki: K)
    {
        
        ++curid;
        if(prt >> (curid + 1) & 1)
        {
            ++ret;
            continue;
        }
        // if(vq[curid][v])
        // {
        //     if(h + disq[curid][v] <= D)
        //         ++ret;
        //     continue;
        // }
        int mi;
        if(hop_vtog[curid + 1][v] != -1 && algo_typ > 1)
            mi = hop_vtog[curid + 1][v]; 
        else
        {
            mi = qhbll[curid + 1].GET_UD(dhbll[D/2-1].L[v], dhbll[D/2-1].nxtl[v]);
            hop_vtog[curid + 1][v] = mi;
            res_vtog.emplace_back(curid, v);
        }    
        
        
        // vq[curid][v] = 1;
        // disq[curid][v] = mi;

        if(h + mi <= D)
            ++ret;
    }
    return ret;
}

pair <vector <int>, double> CBA::Pruned_MaxM(Graph &G, vector <vector <int> > &K, int D, int v)
{
    //fprintf(stderr, "In MaxM: v is %d\n", v);
    vector <int> M;
    double sum = 0;
    int curid = -1;
    for(auto Ki: K)
    {
        ++curid;
        if(hop_vtog[curid + 1][v] > D/2 && algo_typ > 1) continue;
        //if(vq[curid][v]&&disq[curid][v]>D/2) continue;
        int u;
        double mi = INF;
        tie(u, mi) = qhbll[curid + 1].GET_WD(D/2, dhbll[D/2-1].L[v]);

        if(abs(mi - INF) > eps)
        {
            sum += mi;
            if(sum > INF) sum = INF;
            M.push_back(u);
        }    
    }

    // fprintf(stderr, "In MaxM: M is:\n");
    // fprintf(stderr, "sum is %d\n", sum);
    // fprintf(stderr, "rt is %d\n", v);
    // for(auto x:M) 
    //     fprintf(stderr, "%d ", x);
    // fprintf(stderr, "\n\n");

    return make_pair(M, sum);
}

pair <vector <int>, int> CBA::Pruned_OPT_MC(Graph &G, vector <vector <int> > &K, int D)
{
    //hbll.clearbuc();
    int n = G.n;
    // disq.clear(), vq.clear(), miq.clear(), vmq.clear();
    // disq.resize(K.size(), vector<int>(n+1, 0));
    // vq.resize(K.size(), vector<bool>(n+1, false));
    // miq.resize(n+1, 0);
    // vmq.resize(n+1, false);

    vector <int> checked(n + 1), mhp(n + 1, inf);
    vector <double> dis(n + 1, 1e18);
    priority_queue<qnode> Q;
    
    map <int,int> inter;
    int curid = 0;
    for(auto Ki: K) 
    {
        ++curid;
        for(auto k: Ki)
            inter[k]|=1<<curid;
    }

    for(auto [u, prt]: inter)
    {
        Q.push(qnode(u, 0, getpri(K, u, 0, D, prt), prt));
        //fprintf(stderr, "u: %d pri: %d\n", u, getpri(K, u, 0, D));
        mhp[u] = 0;
        dis[u] = 0;
    }

    vector <int> M;
    double Msum = 0;
    int c = 0;
    
    int pricnt[12]={};

    int qpcnt = 0;
    int cutcnt = 0, dcutcnt = 0, pdcutcnt = 0;
    int g = K.size();
    while(!Q.empty())
    {
        ++qpcnt;
        auto qnow = Q.top();
        Q.pop();
        int v = qnow.v, h = qnow.h, priv = qnow.priv, prt = qnow.prt;
        pricnt[priv]++;
        int Ms = M.size();
        if(priv < Ms) break;
        if(priv == Ms)
        {
            // v 为根的最小距离，在 D/2 范围内
            // double mi = INF;
            // if(vmq[v])
            // {
            //     mi = miq[v];
            // }
            // else
            // {
            //     mi = qhbll[0].GET_WD(D/2, dhbll[D/2-1].L[v]).second;
                
            //     // for(auto Ki: K)
            //     // {
            //     //     for(auto k: Ki)
            //     //         mi = min(mi, hbll.GET_WD(v, k, D/2));
            //     // }
            //     vmq[v] = true;
            //     miq[v] = mi;
            // }
            if(dis[v] * Ms >= 1.0 * Msum && algo_typ > 1)
            {
                ++dcutcnt;
                continue;
            }
            double mi;
            if(dis_vtoall[v] > 0)
                mi = dis_vtoall[v];
            else
                mi = dis_vtoall[v] = qhbll[0].GET_WD(D/2, dhbll[D/2-1].L[v]).second;

            if(mi * Ms >= 1.0 * Msum)
            {
                ++cutcnt;
                continue;
            }    
        }
        if(!checked[v])
        {
            // vector <int> Mv;
            // double sum;
            auto [Mv, sum] = Pruned_MaxM(G, K, D, v);

            // fprintf(stderr, "in check: %d\n", v);
            // for(auto x: Mv) fprintf(stderr, "%d ", x);
            // fprintf(stderr, "\neval: %.6lf\n\n", sum);

            checked[v] = true;
            if(Mv.size() > M.size() || (Mv.size() == M.size() && sum < Msum) )
            //&& get_sum(G, Mv, v, D) < get_sum(G, M, c, D)))
            {
                M = Mv;
                c = v;
                Msum = sum;
            }
        }
        if(h < D/2)
        {
            for(auto [nxt, w]: G.ed[v])
            {
                dis[nxt] = min(dis[nxt], dis[v] + w);
                if(h + 1 < mhp[nxt])
                {
                    if(M.size() == g && dis[nxt] * g > Msum && algo_typ > 1)
                    {
                        ++pdcutcnt;
                        continue;
                    }
                    Q.push(qnode(nxt, h + 1, getpri(K, nxt, h + 1, D, prt), prt));
                    mhp[nxt] = h + 1;
                }
            }
        }
    }
    for(auto [cid, v]: res_vtog)
        hop_vtog[cid + 1][v] = -1;
    res_vtog.clear();

    // fprintf(stderr, "ret is :%lu %d\n", M.size(), c);
    // for(auto x: M) fprintf(stderr, "%d ", x);
    // fprintf(stderr, "\n");

    // fprintf(stderr, "In omtmc, pqcnt: %d\n", qpcnt);
    // fprintf(stderr, "group size:%lu\n", K.size());
    // fprintf(stderr, "cut cnt:%d\n", cutcnt);
    // fprintf(stderr, "dcut cnt:%d\n", dcutcnt);
    // fprintf(stderr, "pdcut cnt:%d\n", pdcutcnt);

    // long long cntud = 0, cntwd = 0, sumud = 0, sumwd = 0;
    // for(int i = 0; i <= K.size(); i++) 
    // {
    //     cntud += qhbll[i].cntud, cntwd += qhbll[i].cntwd;
    //     sumud += qhbll[i].sumud, sumwd += qhbll[i].sumwd;
    // }
    // fprintf(stderr, "cnt:%lld %lld\nsum:%lld %lld\nratio:%.3lf %.3lf\n",cntud, cntwd, sumud, sumwd, 1.0*sumud/cntud, 1.0*sumwd/cntwd);

    // // int primx = 0;
    // // for(int i = 0; i < 11; i++) if(pricnt[i] > 0) primx = i;
    // // fprintf(stderr, "pricnt:\n");
    // // for(int i = 0; i <= primx; i++) fprintf(stderr, "%d%c", pricnt[i], " \n"[i == primx]);
    // fprintf(stderr, "\n");

    return make_pair(M, c);
}