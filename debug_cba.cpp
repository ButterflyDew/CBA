#include "CBA.h"
#include "Hub-Labeling/global.h"
#include <cstdio>
Graph G;
CBA cba;
vector<int> extractIntegers(const string& input) 
{
    vector<int> integers;
    istringstream iss(input);
    string token;

    while (iss >> token) 
    {
        try 
        {
            int number = stoi(token);
            integers.push_back(number);
        } catch (const invalid_argument&) 
        {
            cerr << "not a number" << endl;
        }
    }

    return integers;
}

void readQ(vector <vector <int> > &Query)
{
    string filename = "Data/Query.in";

    ifstream inputFile(filename);

    if (!inputFile.is_open()) 
        cerr << "Can't open file: " << filename << endl;

    string line;
    getline(inputFile, line);

    int n = extractIntegers(line)[0];

    while (getline(inputFile, line)) 
    {
        //cout << line << endl;
        auto num = extractIntegers(line);
        Query.push_back(num);
    }
    if(Query.size() != n)
        cerr << "invalid query" << endl;
    inputFile.close();
}
void check_cba()
{
    // int n = 4, m = 5, M = 3;
    // G.random_graph_nm(n, m, M);
    G.read();
    //int n = G.n;
    //int D = generateRandomNumber(1, n/2);
    int D = 4;

    vector <vector <int> > Query;
    readQ(Query);

    // G.Print();
    // for(auto qk: Query)
    // {
    //     for(auto k: qk)
    //         printf("%d ",k);
    //     printf("\n");
    // }

    Tree T = cba.Go_CBA(G, Query, D);
    T.Print();
}

int main()
{
    check_cba();
    return 0;
}