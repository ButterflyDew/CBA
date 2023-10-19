#include "CBA.h"

vector <int> CBA::Go_CBA(Graph G, vector <vector <int> > K, int D)
{
    auto ret = OPT_MC(G, K, D);
    vector <int> M = ret.first;
    int c = ret.second;

    vector <int> S = M;
    sort(S.begin(), S.end());
    S.erase(std::unique(S.begin(), S.end()), S.end());

    vector <int> Vt;
    Vt.push_back(c);
    vector <pair <int,int> > Et;

    
}
