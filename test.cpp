#include <bits/stdc++.h>
#define FOR(i,a,b) for(int i=(a);i<=(b);i++)
#define ll long long
//2024041331404202
using namespace std;
int fp[210000],fm[210000];
int main()
{
    //freopen("B.out","w",stdout);
    fm[0]=1;
    ll ans=0;
    FOR(i,1,99)
    {
        fp[i]=(fp[i-1]+i)%100;
        fm[i]=(fm[i-1]*i)%100;
        //printf("%d %d\n",fp[i],fm[i]);
        // if(fp[i]==0)
        // {
        //     printf("%d\n",i);
        // }
        if(fp[i]==fm[i]) ++ans;
    }
    ll sum=2024041331404202ll/100ll;
    sum=(sum-1ll)*2 + 1;
    printf("%lld %lld\n",ans,ans+sum);
    return 0;
}