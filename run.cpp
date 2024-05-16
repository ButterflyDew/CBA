#include "CBA.h"
#include "core.h"
#include "Hub-Labeling/global.h"
#include "Hub-Labeling/hbll.h"
#include <cstdio>
#include <string>
Graph G;
CBA cba;
Core core;
string filepre, filepreout;

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

void Print_time_only(string filepre, int qid, vector <double> t_optmc, vector <double> t_loop)
{
    ios_base::openmode mode = ios::app;
    if (qid == 0) 
        mode = ios::trunc;

    ofstream outputFile(filepre + "/time_result_only.txt", mode);
    if (!outputFile.is_open()) {
        cerr << "Error: Couldn't open the file." << endl;
        return;
    }
    
    if(qid == -1)
    {
        outputFile << "Aver:\n";
        for(int i = 0; i < 3; i++)
            outputFile << t_optmc[i] << " \n"[i == 2];
        outputFile.close();
        return;
    }

    //outputFile << "------------------query " << qid << "------------------" << endl;

    for(int i = 0; i < 3; i++)
    {
        double le = t_optmc[i], ri = t_loop[i];
        outputFile << le + ri << " \n"[i == 2];
    }
    outputFile.close();
}

//synthetic
void readQ(vector <vector <vector <int> > > &Queryset)
{
    string filename = filepre + "/queryList.txt";

    ifstream inputFile(filename);

    if (!inputFile.is_open()) 
        cerr << "Can't open file: " << filename << endl;

    string line;

    while (getline(inputFile, line)) 
    {
        vector <vector <int> > Query;
        istringstream lines(line);
        string lin;
        while(getline(lines, lin, ','))
        {
            auto num = extractIntegers(lin);
            Query.push_back(num);
        }    
        Queryset.push_back(Query);
    }
    inputFile.close();
}

/*
real
nodeName.txt: Mapping from vertex ID to vertex name (i.e., entity URI).
query.txt: Each line is a keyword query containing a set of keyword names.
kwName.txt: Mapping from keyword ID to keyword name.
kwMap.txt: Mapping from keyword ID to vertex IDs. The first value of each line is keyword ID, and the rest are vertex IDs.
keyword ID -> keyword name
keyword ID -> vertex IDs
*/
string transl(string str)
{
    transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return tolower(c); });
    return str;
}
map <string, int> get_knm_to_kid()
{
    string filename = filepre + "/kwName.txt";
    //cerr << "kwName: " << filename << endl;
    ifstream inputFile(filename);
    if (!inputFile.is_open()) 
        cerr << "Can't open kwName: " << filename << endl;
    
    map <string, int> knm_kid;
    string line;
    while (getline(inputFile, line)) 
    {
        //cerr << line << endl;
        istringstream iss(line);
        int kid;
        string knm;
        iss >> kid >> knm;   
        knm = transl(knm);
        knm_kid[knm] = kid;
    }
    inputFile.close();
    return knm_kid;
}

map <int, vector <int> > get_kid_to_vid()
{
    string filename = filepre + "/kwMap.txt";
    ifstream inputFile(filename);
    if (!inputFile.is_open()) 
        cerr << "Can't open kwMap: " << filename << endl;
    
    map <int, vector <int> > kid_vid;
    string line;
    while (getline(inputFile, line)) 
    {
        istringstream iss(line);
        int kid, vid;
        iss >> kid;
        vector <int> svid;
        while(iss >> vid)
            svid.push_back(vid);
        kid_vid[kid] = svid;
    }
    inputFile.close();
    return kid_vid;
}

void readQR(vector <vector <vector <int> > > &Queryset)
{
    //cerr << "Start readQR" << endl;
    auto knm_kid = get_knm_to_kid();
    cerr << "Finished kwName" << endl;
    auto kid_vid = get_kid_to_vid();
    cerr << "Finished kwMap" << endl;

    string filename = filepre + "/query.txt";
    ifstream inputFile(filename);
    if (!inputFile.is_open()) 
        cerr << "Can't open query: " << filename << endl;

    string line;
    while (getline(inputFile, line)) 
    {
        vector <vector <int> > Query;
        istringstream iss(line);
        string knm;
        while(iss >> knm)
        {
            knm = transl(knm);
            if(knm_kid.find(knm) == knm_kid.end())
            {
                cerr << "Name " << knm << " don't hava kid!" << endl;
                continue;
            }
            if(kid_vid.find(knm_kid[knm]) == kid_vid.end())
            {
                cerr << "Kid " << knm_kid[knm] << "don't have node set!" << endl;
                continue;
            }
            Query.push_back(kid_vid[knm_kid[knm]]);
        }
        Queryset.push_back(Query);
    }
    inputFile.close();
}
//Mondial UW 0
map <string, int> dataname, edgetype, algotype;
void init()
{
    dataname["MND"] = 1;
    dataname["Mondial"] = 1;
    dataname["OpenCyc"] = 2;
    dataname["CYC"] = 2;
    dataname["YAGO"] = 3;
    dataname["LMDB"] = 4;
    dataname["LinkedMDB"] = 4;
    dataname["DBP"] = 5;
    dataname["Dbpedia"] = 5;
    dataname["LUBM-50K"] = 6;
    dataname["LUBM-500K"] = 7;
    dataname["LUBM-5M"] = 8;
    dataname["testdata"] = 9;

    edgetype["UW"] = 1;
    edgetype["IW"] = 2;

    algotype["core"] = 1;
    algotype["core+"] = 2;
    algotype["CBA"] = 3;
    algotype["CBA+"] = 4;
    algotype["CBAf"] = 5;
    algotype["CBAHash"] = 6;
}
//testdata UW 1
//LUBM-50K UW 0
string filehbll, filehbllpre = R"(C:\Users\Dew\Desktop\Dew\HBLL_resource)";
string filehblltran;
void read_pram(string &fordername, string &graphname, string &algoname, int &dataid)
{
    filehbllpre = R"(C:\Users\Dew\Desktop\Dew\HBLL_resource)";
    fprintf(stderr, "e.g. LUBM-50K IW CBA+\n");
    string ss;
    cin >> ss;
    if(dataname.find(ss) == dataname.end())
    {
        fprintf(stderr, "illegal data!");
        exit(0);
    }
    dataid = dataname[ss]; 
    fordername = "/" + ss;
    filepre = "resource" + fordername;
    filehbllpre += "\\" + ss;

    cin >> ss;
    if(edgetype.find(ss) == edgetype.end())
    {
        fprintf(stderr, "illegal edgetype!");
        exit(0);
    }
    if(edgetype[ss] == 1)
    {
        graphname = filepre + "/graph.txt";
        filepreout = filepre + "/UW";
        filehbll = filehbllpre + "\\UWHBLL.txt";
        filehblltran = filepre + "/uwhbll.txt";
    }    
    else
    {
        graphname = filepre + "/Weightgraph.txt";
        filepreout = filepre + "/IW";
        filehbll = filehbllpre + "\\IWHBLL.txt";
        filehblltran = filepre + "/iwhbll.txt";
    }

    cin >> algoname;
    if(algotype.find(algoname) == algotype.end())
    {
        fprintf(stderr, "illegal algotype!");
        exit(0);
    }
    filepreout += "_" + algoname;
    
    //filepreout += "w_o_prev.";

    if (!filesystem::exists(filepreout)) 
    {
        try 
        {
            filesystem::create_directory(filepreout);
        } 
        catch (const filesystem::filesystem_error& e) 
        {
            cerr << "Error creating folder: " << e.what() << '\n';
        }
    }

    
}

void read_hbll_pram(string namet, string edget)
{
    filehbllpre = R"(C:\Users\Dew\Desktop\Dew\HBLL_resource)";
    cerr << "Start go " << namet << " " << edget << "\n";
    string ss, info;
    //cin >> ss;
    ss = namet;
    if(dataname.find(ss) == dataname.end())
    {
        fprintf(stderr, "illegal data!");
        exit(0);
    }
    filehbllpre += "\\" + ss;
    info = ss;
    filepre = "resource/" + ss;
    //cin >> ss;
    ss = edget;
    if(edgetype.find(ss) == edgetype.end())
    {
        fprintf(stderr, "illegal edgetype!");
        exit(0);
    }
    info += " " + ss;
    if(edgetype[ss] == 1)
    {
        filehbll = filehbllpre + "\\UWHBLL.txt";
        filehblltran = filepre + "/uwhbllinfo.txt";
    }    
    else
    {
        filehbll = filehbllpre + "\\IWHBLL.txt";
        filehblltran = filepre + "/iwhbllinfo.txt";
    }    

    HBLL hbll;
    hbll.go_info(filehbll, "resource", info);
}

void read_hbll_pram()
{
    vector <string> namlist, edglist;
    edglist.push_back("UW");
    edglist.push_back("IW");
    namlist.push_back("OpenCyc");
    namlist.push_back("Mondial");
    namlist.push_back("LUBM-50K");
    namlist.push_back("YAGO");
    namlist.push_back("LinkedMDB");
    namlist.push_back("LUBM-500K");
    namlist.push_back("Dbpedia");
    namlist.push_back("LUBM-5M");
    
    for(auto name: namlist)
        for(auto edge: edglist)
            read_hbll_pram(name, edge);
}

void urn_algo()
{
    string fordername, graphname, algoname;
    int dataid;
    read_pram(fordername, graphname, algoname, dataid);
    
    G.read(graphname, 0);
    if(!filesystem::exists(filehblltran))
    {
        fprintf(stderr, "Start translating HBLL!\n");
        HBLL hbll;
        hbll.read(filehbll);
        hbll.output_F(filehblltran);
        fprintf(stderr, "HBLL is translated!\n");
    }

    vector <vector <vector <int> > > Queryset;
    if(dataid >= 6)
        readQ(Queryset);
    else
        readQR(Queryset);
    fprintf(stderr, "Query is read\n");

    if(algotype[algoname] >= 3)
        cba.read_hbll(filehblltran);
    else if(algotype[algoname] == 1)
        core.read_hbll(filehblltran);
    fprintf(stderr, "HBLL is read\n");

    int qid  = 0;
    vector <double> t_sum(3, 0), t_empty(3, 0);
    for(auto Query: Queryset)
    {
        fprintf(stderr, "------------------query %d------------------\n", qid);
        fprintf(stderr, "Start go %d\n", qid);
        int siz_n = 0, siz_q = 0;
        for(auto Q: Query)
            siz_q += Q.size(), siz_n ++;
        vector <double> t_optmc, t_loop;
        
        double aver_l;
        for(int D = 2; D <= 6; D += 2)
        {
            if(algotype[algoname] >= 3)
            {
                cba.hbll.clearcnt();
                Tree T = cba.Go_CBA(G, Query, D, algotype[algoname]);
                t_optmc.push_back(cba.after_optmc_time - cba.start_time);
                t_loop.push_back(cba.end_time - cba.after_optmc_time);
                T.Print_to_file(filepreout, qid, D, cba.revM);
                aver_l = cba.aver_l;
            }
            else if(algotype[algoname] == 1)
            {
                core.hbll.clearcnt();
                Tree T;

                T = core.Go_Core(G, Query, D);
                t_optmc.push_back(core.end_time - core.start_time);  
                // Tree T = core.Go_Core(G, Query, D);
                // t_optmc.push_back(core.end_time - core.start_time);
                t_loop.push_back(0);
                T.Print_to_file(filepreout, qid, D, core.revM);
            }
            fprintf(stderr, "D = %d done!\n", D);
        }
        Print_time_all(filepreout, qid, t_optmc, t_loop);
        Print_time_only(filepreout, qid, t_optmc, t_loop);
        for(int i = 0; i < 3; i++)
            t_sum[i] += t_optmc[i] + t_loop[i];
        ++qid;
    }    
    for(int i = 0; i < 3; i++)
        t_sum[i] = t_sum[i]/qid;
    Print_time_only(filepreout, -1, t_sum, t_empty);
}

// void go_cba()
// {
//     string fordername, graphname;
//     int ishbll = 0, dataid;
//     read_pram(fordername, graphname, ishbll, dataid, 0);
    
//     G.read(graphname, 0);
//     if(!filesystem::exists(filepreout + "/hbll.txt") || ishbll)
//     {
//         fprintf(stderr, "Start translating HBLL!\n");
//         // HBLL hbll;
//         // hbll.build_hbll(G);
//         HBLL hbll;
//         hbll.read(filehbll);
//         hbll.output_F(filepreout);
//         fprintf(stderr, "HBLL is translated!\n");
//     }

    
//     vector <vector <vector <int> > > Queryset;
//     if(dataid >= 6)
//         readQ(Queryset);
//     else
//         readQR(Queryset);

//     cba.read_hbll(filepreout);
//     fprintf(stderr, "HBLL is read\n");
//     int qid = 0;

//     for(auto Query: Queryset)
//     {
//         fprintf(stderr, "Start go %d\n", qid);
//         int siz_n = 0, siz_q = 0;
//         for(auto Q: Query)
//             siz_q += Q.size(), siz_n ++;
//         vector <double> t_optmc, t_loop;
//         double aver_l;
//         for(int D = 2; D <= 6; D += 2)
//         {
//             cba.hbll.clearcnt();
//             //cba.hbll.cntud = cba.hbll.cntwd = 0;
//             Tree T = cba.Go_CBA(G, Query, D);
//             t_optmc.push_back(cba.after_optmc_time - cba.start_time);
//             t_loop.push_back(cba.end_time - cba.after_optmc_time);
//             T.Print_to_file(filepreout, qid, D, cba.revM);
//             aver_l = cba.aver_l;
//             //cba.hbll.printcnt();
//             fprintf(stderr, "D = %d done!\n", D);
//         }
//         Print_time(filepreout, qid, siz_n, siz_q, aver_l, t_optmc, t_loop);
//         Print_time_all(filepreout, qid, t_optmc, t_loop);
//         Print_time_only(filepreout, qid, t_optmc, t_loop);
//         ++qid;

//         //break;
//     }    
// }

// void go_core()
// {
//     string fordername, graphname;
//     int ishbll = 0, dataid;
//     read_pram(fordername, graphname, ishbll, dataid, 1);
    
//     G.read(graphname, 0);

//     string filehbll;
//     if(!filesystem::exists(filepreout + "/hbll.txt") || ishbll)
//     {
//         fprintf(stderr, "Start building HBLL!\n");
//         HBLL hbll;
//         hbll.build_hbll(G);
//         //hbll.read(filehbll);
//         hbll.output_F(filepreout);
//         fprintf(stderr, "HBLL is built!\n");
//     }
    
//     vector <vector <vector <int> > > Queryset;
//     if(dataid >= 6)
//         readQ(Queryset);
//     else
//         readQR(Queryset);

//     core.read_hbll(filepreout);
//     fprintf(stderr, "HBLL is read\n");
//     int qid = 0;

//     for(auto Query: Queryset)
//     {
//         fprintf(stderr, "Start go %d\n", qid);
//         int siz_n = 0, siz_q = 0;
//         for(auto Q: Query)
//             siz_q += Q.size(), siz_n ++;
//         vector <double> t_core, t_tmp;
//         for(int D = 2; D <= 6; D += 2)
//         {
//             core.hbll.clearcnt();
//             Tree T = core.Go_Core(G, Query, D);
//             t_core.push_back(core.end_time - core.start_time);
//             t_tmp.push_back(0);
//             T.Print_to_file(filepreout, qid, D, core.revM);
//             fprintf(stderr, "D = %d done!\n", D);
//         }
//         Print_time_only(filepreout, qid, t_core, t_tmp);
//         ++qid;

//         //break;
//     }    
// }

int main()
{
    init();
    read_hbll_pram();
    //run_algo();
    //go_cba();
    //go_core();
    return 0;
}