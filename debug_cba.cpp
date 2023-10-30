#include "CBA.h"
#include "Hub-Labeling/global.h"
#include "Hub-Labeling/hbll.h"
#include <cstdio>
#include <string>
Graph G;
CBA cba;
string filepre;

void readQ(vector <vector <int> > &Query)
{
    string filename = filepre + "/queryList.txt";
    //string filename = filepre + "/testQ.txt";

    ifstream inputFile(filename);

    if (!inputFile.is_open()) 
        cerr << "Can't open file: " << filename << endl;

    string line;
    //getline(inputFile, line);

    //int n = extractIntegers(line)[0];

    while (getline(inputFile, line)) 
    {
        //cout << line << endl;
        istringstream lines(line);
        string lin;
        while(getline(lines, lin, ','))
        {
            //cout << "set " << lin << endl;
            auto num = extractIntegers(lin);
            Query.push_back(num);
        }    
    }
    // if(Query.size() != n)
    //     cerr << "invalid query" << endl;
    inputFile.close();
}
void check_cba()
{
    // int n = 4, m = 5, M = 3;
    // G.random_graph_nm(n, m, M);
    //int n = G.n;
    //int D = generateRandomNumber(1, n/2);


    filepre = "PaperData/testdata";
    G.read(filepre+"/graph.txt", 0);
    int D = 4;
    HBLL hbll;
    hbll.build_hbll(G);
    hbll.output_F(filepre);

    fprintf(stderr, "HBLL is built!\n");

    vector <vector <int> > Query;
    readQ(Query);

    // G.Print();
    // for(auto qk: Query)
    // {
    //     for(auto k: qk)
    //         printf("%d ",k);
    //     printf("\n");
    // }
    cba.read_hbll(filepre);
    //fprintf(stderr, "hbll is read\n");
    Tree T = cba.Go_CBA(G, Query, D);
    T.Print();
}

int main()
{
    check_cba();
    return 0;
}