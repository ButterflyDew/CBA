#include "CBA.h"
#include "Hub-Labeling/global.h"
#include "Hub-Labeling/hbll.h"
#include <cstdio>
#include <string>
Graph G;
CBA cba;
string filepre;

void readQ(vector <vector <vector <int> > > &Queryset)
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
        vector <vector <int> > Query;
        //cout << line << endl;
        istringstream lines(line);
        string lin;
        while(getline(lines, lin, ','))
        {
            //cout << "set " << lin << endl;
            auto num = extractIntegers(lin);
            Query.push_back(num);
        }    
        Queryset.push_back(Query);
    }
    // if(Query.size() != n)
    //     cerr << "invalid query" << endl;
    inputFile.close();
}

void Print_time(string filepre, int qid, int siz_n, int siz_q, double aver_l, vector <double> t_optmc, vector <double> t_loop)
{
    string folderName = filepre + '/' +to_string(qid);
    ofstream outputFile(folderName + "/time_result.txt");
    outputFile << "The average siz of HBLL is: " << aver_l << endl;
    outputFile << "The point of Q is: " << siz_q << endl;
    outputFile << "The query of Q is: " << siz_n << endl;
    for(int i = 0; i < 3; i++)
    {
        double le = t_optmc[i], ri = t_loop[i];
        outputFile << "D = " << (i+1)*2 << ": optmc is " << le << "ms, loop is " << ri << " ms." << endl;
        outputFile << "Ratio = " << le/(le + ri) << endl;
    } 
    outputFile.close();
}

void Print_time_all(string filepre, int qid, vector <double> t_optmc, vector <double> t_loop)
{
    ios_base::openmode mode = ios::app;
    if (qid == 0) 
        mode = ios::trunc;

    ofstream outputFile(filepre + "/time_result_all.txt", mode);
    if (!outputFile.is_open()) {
        cerr << "Error: Couldn't open the file." << endl;
        return;
    }
    
    outputFile << "------------------query " << qid << "------------------" << endl;
    for(int i = 0; i < 3; i++)
    {
        double le = t_optmc[i], ri = t_loop[i];
        outputFile << "diameter=" << (i+1)*2 << "   time: " << le + ri << " ms" << endl;
    }

    outputFile.close();
}

void testv()
{
    double st = get_now_time();
    int n = cba.hbll.n, ct = 0;
    for(int i = 1; i <= 1000000; i++)
    {
        int u = generateRandomNumber(1, n);
        int v = generateRandomNumber(1, n);
        int h = generateRandomNumber(1, 10);
        ct += cba.hbll.L[u].size();
        ct += cba.hbll.L[v].size();
        cba.hbll.GET_WD(u, v, h);
    }
    double ed = get_now_time();
    fprintf(stderr, "%.0lf ms si: %d\n", ed - st, ct);
    exit(0);
}

string fordername = "/LUBM-50K_Weight";
//string fordername = "/testdata-d";
void go_cba()
{
    filepre = "PaperData" + fordername;
    //G.read(filepre+"/graph.txt", 0);
    G.read(filepre+"/Weightgraph.txt", 0);
    if(!filesystem::exists(filepre + "/hbll.txt"))
    {
        HBLL hbll;
        hbll.build_hbll(G);
        hbll.output_F(filepre);
        fprintf(stderr, "HBLL is built!\n");
    }
    
    vector <vector <vector <int> > > Queryset;
    readQ(Queryset);

    cba.read_hbll(filepre);
    fprintf(stderr, "hbll is read\n");
    int qid = 0;

    //double aver = hbll.Average_L();
    //testv();

    for(auto Query: Queryset)
    {
        if(qid == 1)
        {
            ++qid;
            break;
            //continue;
        }
        fprintf(stderr, "Start go %d\n", qid);
        int siz_n = 0, siz_q = 0;
        for(auto Q: Query)
            siz_q += Q.size(), siz_n ++;
        vector <double> t_optmc, t_loop;
        double aver_l;
        for(int D = 2; D <= 6; D += 2)
        {
            cba.hbll.clearcnt();
            //cba.hbll.cntud = cba.hbll.cntwd = 0;
            Tree T = cba.Go_CBA(G, Query, D);
            t_optmc.push_back(cba.after_optmc_time - cba.start_time);
            t_loop.push_back(cba.end_time - cba.after_optmc_time);
            T.Print_to_file(filepre, qid, D, cba.revM);
            aver_l = cba.aver_l;
            cba.hbll.printcnt();
            fprintf(stderr, "D = %d done!\n", D);
        }
        Print_time(filepre, qid, siz_n, siz_q, aver_l, t_optmc, t_loop);
        Print_time_all(filepre, qid, t_optmc, t_loop);
        ++qid;

        //break;
    }    
}

int main()
{
    go_cba();
    return 0;
}