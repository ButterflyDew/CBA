#include "testlib.h"
#include <bits/stdc++.h>
#define FOR(i,a,b) for(int i=(a);i<=(b);i++)
using namespace std;
#define ll long long
double r1,r2,r3,r4;
const int N=2e5+10;
int n,q;
int tot,ch[N][2],fa[N],rt;
int dfn[N],dat[N],clo;
int mxv=1e9;
int vv=1e9;
void cons_t(int &now,int par,int res)
{
    if(res==0) return;
    if(now==0) now=++tot;
    dat[now]=rnd.next(1,mxv);
    fa[now]=par;
    --res;
    int siz;
    if(res<=10)
        siz=rnd.next(1,res);
    else
    {
        double p=rnd.next(1.0);
        if(p<=r1) siz=res;
        else if(p<=r1+r2) siz=rnd.next(0,(int)(log2(res)));
        else if(p<=r1+r2+r3+r4) siz=rnd.next(0,(int)(sqrt(res)));
        else siz=rnd.next(0,res);
    }
    int chi=rnd.next(0,1);
    cons_t(ch[now][chi],now,siz);
    cons_t(ch[now][chi^1],now,res-siz);
}
ll sum[N];
int siz[N];
void dfs(int now)
{
    dfn[++clo]=now;
    siz[now]=1;
    sum[now]=0;
    if(ch[now][0]) 
    {
        dfs(ch[now][0]);
        sum[now]+=sum[ch[now][0]]+siz[ch[now][0]]*(dat[now]-dat[ch[now][0]]);
        siz[now]+=siz[ch[now][0]];
    }
    if(ch[now][1]) 
    {
        dfs(ch[now][1]);
        sum[now]+=sum[ch[now][1]]+siz[ch[now][1]]*(dat[now]-dat[ch[now][1]]);
        siz[now]+=siz[ch[now][1]];
    }
}
int main(int argc, char *argv[]) {
    registerGen(argc, argv, 1);
    //r1: 一侧空 r2: 一侧拿log r3: 一侧拿根号 r4: 随机
    double r1 = atoi(argv[1]);
    double r2 = atoi(argv[2]);
    double r3 = atoi(argv[3]);
    double r4 = atoi(argv[4]);
    mxv=atoi(argv[5]);
    n = 200000, q = 200000;
    cons_t(rt, 0, n);
    FOR(i,1,n)
    {
        int now=i;
        while(fa[now]&&dat[fa[now]]<dat[now])
        {
            swap(dat[fa[now]],dat[now]);
            now=fa[now];
        }
    }
    dfs(rt);
    sort(sum+1,sum+1+n);
    int m=0;
    while(m<=n&&sum[m+1]<=vv) ++m;
    sum[m+1]=vv+1;
    printf("%d\n",n);
    FOR(i,1,n) printf("%d%c",dat[dfn[i]]," \n"[i==n]);
    printf("%d\n",q);
    FOR(i,1,q)
    {
        int pos=rnd.next(1,n);
        int posv=rnd.next(0,m);
        int v=rnd.next(sum[posv],sum[posv+1]-1);
        if(v%2==1) --v;
        if(v<2) v=2;
        printf("%d %d\n",pos,v);
    }
}
