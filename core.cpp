#include "core.h"
#include "Hub-Labeling/global.h"
#include <array>
#include <cstdio>
#include <queue>
#include <string>
Tree Core::Go_Core(Graph G, vector <vector <int> > K, int D)
{
    start_time = get_now_time();
    Tree T(K.size(), 0);
    set <int> pot;
    for(auto ki: K)
        for(auto u: ki)
            pot.insert(u);
    //fprintf(stderr, "1\n");
    map <int, map <int,int>> vis;
    //vector <vector <int> > vis(pot.size() + 2, vector <int> (G.n + 2));
    vector <int> checked(G.n + 2);
    //hbll.GET_UD(9, 9);
    //fprintf(stderr, "2\n");
    priority_queue <array <int, 3>, vector <array <int, 3> >, greater<array <int, 3> > > pq;
    for(auto u: pot)
    {
        //fprintf(stderr, "%d\n", u);
        vis[u][u] = 1;
        pq.push({getpri(K, u, u, D), u, u});
    }    
    fprintf(stderr, "finish core pre\n");
    while(!pq.empty())
    {
        if(get_now_time() - start_time > 1000*50) break;
        auto [_, u, v] = pq.top();
        pq.pop();
        if(getpri(K, u, v, D) >= T.dor) 
            break;
        // fprintf(stderr, "u,v: %d %d\n",u,v);
        // fprintf(stderr, "ok1\n");
        if(checked[v] == 0)
        {
            auto T_ = FindAns(G, K, D, v, T);
            checked[v] = 1;
            if(T_.n != 0)
                T = T_;
        }
        //fprintf(stderr, "ok2\n");
        if(hbll.GET_UD(u, v) < D/2)
        {
            for(auto [w, _]: G.ed[v])
            {
                if(vis[u][w] == 0)
                {
                    vis[u][w] = 1;
                    pq.push({getpri(K, u, w, D), u, w});
                }
            }
            //fprintf(stderr, "ok3\n");
        }
            
    }
    fprintf(stderr, "pq end!\n");
    end_time = get_now_time();
    T.gval = G.gval;
    revM = T.revM;
    return T;
}

Tree Core::FindAns(Graph &G, vector <vector <int> > &K, int D, int v, Tree opt)
{
    int cnt = 0;
    vector <int> isu(K.size(), -1);
    int i = 0;
    for(auto ki: K)
    {
        for(auto u: ki)
            if(hbll.GET_UD(u, v) <= D/2)
            {
                ++cnt;
                isu[i] = u;
                break;
            }
        ++i;
    }
    if(cnt > opt.CQ)
        return GenAnsEven(G, K, D, v, isu);
    else 
        return Tree();
}

Tree Core::GenAnsEven(Graph &G, vector <vector <int> > &K, int D, int v, vector <int> isu)
{
    Tree T(K.size(), 0);
    int i = 0;
    for(auto ki: K)
    {
        int u = isu[i];
        // for(auto u_: ki)
        //     if(hbll.GET_UD(u_, v) <= D/2)
        //     {
        //         u = u_;
        //         break;
        //     }
        if(u != -1)
        {
            auto path = hbll.GET_MWP(v, u, D/2);
            Expand(T, v, path);
            T.revM.push_back(u);
            T.dor --, T.CQ ++;
        }
        ++i;
    }
    return T;
}

int Core::getpri(vector <vector <int> > &K, int u, int v, int D)
{
    //fprintf(stderr, "%d %d\n", u, v);
    int duv = hbll.GET_UD(u, v), ret = 0;
    for(auto ki: K)
    {
        int flg = 1;
        for(auto ui: ki)
            if(hbll.GET_UD(v, ui) + duv <= D)
            {
                flg = 0;
                break;
            }
        ret += flg;
    }
    //fprintf(stderr, "end getpri\n");
    return ret;
}

void Core::Expand(Tree &T, int c, vector <int> path)
{
    if(T.vex.empty()) T.add_root(c);
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

void Core::read_hbll(string filepre)
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
        while(getline(lines, lin, ','))
        {
            //cout << "set " << lin << endl;
            auto [a, b, c, d] = extract_values(lin);
            hbll.L[hbll.n].push_back(Triple(a, b, c));
            hbll.prew[hbll.n].push_back(d);
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
}