#include <stdio.h>
#include <algorithm>
#include <numeric>
#include <list>
#include <vector>
#include <iterator> //ostream_iterator
#include <iostream> //cout cin
#include <sstream>
#include <fstream>
#include <utility> //std::pair, krawedzie w Graph
#include <set>
#include <functional>

#include <cstdlib>
#include <time.h>

#include <windows.h>

using namespace std;

typedef unsigned long int uint;
typedef pair<uint, uint> PAIRINT;
typedef pair<PAIRINT, uint> EDGE;

namespace std
{
ostream& operator<<(ostream& cout, const EDGE& e)
{
    int src=e.first.first;
    int dest=e.first.second;
    if(dest<src) swap(src,dest);
    cout<<
        //(::odd?"\t":"")<<
        src<<" "<<dest<<" "<<e.second;
    return cout;
}
}

struct unary_f
{
    virtual bool operator()(const EDGE&) =0;
};

class Graph
{
public:

    std::vector<EDGE> edgesHeap;

    friend istream& operator>>(istream& cin, Graph& G)
    {
        int x,y,w;
        int v,e;
        G.clearEdgesList();
        cin>>v>>e;
        printf("wczytano v=%d e=%d\n",v,e);
        G.setNumberOfVerticles(v);
        int i;
        for(i=0; i<e; ++i)
        {
            cin>>x;
            cin>>y;
            if(x>y)
            {
                //std::cerr<<"error in input: first coordinate should be lower\n";
                swap(x,y);
            }
            cin>>w;
            //cout<<"wczytano krawdz "<<x<<" to "<<y<<" for "<<w<<"\n";
            //if(G.polaczenia.capacity()<std::max(x,y)+1)
            //    G.polaczenia.resize(std::max(x,y)+1);
            G.connectNodes((uint)x,(uint)y,(uint)w);
        }
        //G.M=i;
        return cin;
    }

    virtual int getEdgesNumber() const
    {
        return edgesHeap.size();
    }
    virtual void connectNodes(uint from, uint to, uint weight)=0;
    virtual void setNumberOfVerticles(int v)=0;
    virtual int getNumberOfVerticles() const=0;
    virtual void clearEdgesList()=0;
    virtual void iterateThroughNeighbours(int &u, unary_f &fnc) const=0;


    virtual operator std::string() const=0;

    Graph &loadGraphFromFile(const char *fname)
    {
        std::fstream in (fname, fstream::in | fstream::out);
        if(!in.is_open())
        {
            std::cerr<<"file could not be found!\n";
            return *this;
        }
        in>>*this;
        in.close();
        return *this;
    }

    void generateGraph(int v, double fill, int weightMax=100, int weightMin=1)
    {
        clearEdgesList();
        setNumberOfVerticles(v);
        srand(time(0));
        std::vector<pair<int,int> > omega;
        std::vector<pair<int,int> > rest;
        for(int i=0; i<v; ++i)
            for(int j=i+1; j<v; ++j)
            {
                omega.push_back(make_pair(i,j));
            }
        std::vector<int> isIn(v,false);

        std::random_shuffle(omega.begin(),omega.end());
        int cnt=0;
        int e=fill*omega.size();
        for(size_t o=0; o<omega.size(); ++o)
        {
            if(isIn[omega[o].first]==false || isIn[omega[o].second]==false)
            {
                isIn[omega[o].first]=true;
                isIn[omega[o].second]=true;
                connectNodes(omega[o].first, omega[o].second, rand()%(weightMax-weightMin)+weightMin);
                cnt++;
            }
            else
            {
                rest.push_back(omega[o]);
            }
        }
        omega.clear();
        omega=rest;

        for(; cnt<e && omega.size(); cnt++)
        {
            pair<int, int> edge=omega.back();
            omega.pop_back();
            connectNodes(edge.first, edge.second, rand()%(weightMax-weightMin)+weightMin);
        }
        if(cnt<e)
            std::cerr<<"not enough edges\n";
    }
};

class GraphList: public Graph
{
public:
    //std::vector<Vertex > nodes;
    std::vector< list<EDGE> > polaczenia; //tablica list par opisujacych polaczenie [para polaczonych wierzcholkow, wag]

    void connectNodes(uint from, uint to, uint weight)
    {
        EDGE k(PAIRINT(from, to),weight);
        polaczenia[from].push_back(k);
        edgesHeap.push_back(k);
        std::swap(from, to);
        polaczenia[from].push_back(EDGE(PAIRINT(from, to),weight));
    }

    int getEdgesNumber() const
    {
        return edgesHeap.size();
    }
    virtual void setNumberOfVerticles(int v)
    {
        polaczenia.resize(v);
    }
    virtual int getNumberOfVerticles() const
    {
        return polaczenia.size();
    }
    virtual void clearEdgesList()
    {
        int v=getEdgesNumber();
        edgesHeap.clear();
        polaczenia.clear();
        setNumberOfVerticles(v);
    };
    GraphList (int N)
    {
        setNumberOfVerticles(N);
    }
    //iterates through all neighbours of u with function fnc
    //used mainly for relaxation
    //template <class UnaryFunction>
    void iterateThroughNeighbours(int &u, unary_f &fnc) const
    {
        for(std::list<EDGE>::const_iterator it=polaczenia[u].begin(); it!=polaczenia[u].end(); it++)
            fnc(*it);
    };
    operator std::string() const
    {
        std::stringstream ss;
        //ss<<"|G|="<<polaczenia.size()<<"\n";
        string f;
        int i;
        int N=getNumberOfVerticles();
        int M=getEdgesNumber();
        ss<<N<<" "<<M<<endl;
        for(i=0; i<M; ++i)
        {
            ss<<edgesHeap[i]<<endl;
            f=ss.str();
        }
        return ss.str();
    }
};


class GraphMatrix: public Graph
{
public:
    //std::vector<Vertex > nodes;
    std::vector< vector<int> > polaczenia; //tablica list par opisujacych polaczenie [para polaczonych wierzcholkow, wag]

    void connectNodes(uint from, uint to, uint weight)
    {
        EDGE k(PAIRINT(from, to),weight);
        edgesHeap.push_back(k);
        polaczenia[from][to]=weight;
        polaczenia[to][from]=weight;
    }

    int getEdgesNumber() const
    {
        return edgesHeap.size();
    }
    virtual void setNumberOfVerticles(int v)
    {
        polaczenia.resize(v);
        for(int i=0; i<v; ++i)
        {
            polaczenia[i].resize(v);
            std::fill(polaczenia[i].begin(),polaczenia[i].end(),0);
        }
    }
    virtual int getNumberOfVerticles() const
    {
        return polaczenia.size();
    }
    virtual void clearEdgesList()
    {
        edgesHeap.clear();
        polaczenia.clear();
    };
    GraphMatrix (int N)
    {
        setNumberOfVerticles(N);
    }
    //iterates through all neighbours of u with function fnc
    //used mainly for relaxation
    //template <class UnaryFunction>
    void iterateThroughNeighbours(int &u, unary_f &fnc) const
    {
        int n=getNumberOfVerticles();
        for(int i=0; i<n; ++i)
            if(polaczenia[u][i]!=0)
            {
                EDGE a(PAIRINT(u,i),polaczenia[u][i]);
                fnc(a);
            }
    };
    operator std::string() const
    {
        std::stringstream ss;
        //ss<<"|G|="<<polaczenia.size()<<"\n";
        string f;
        int i;
        int N=getNumberOfVerticles();
        ss<<getNumberOfVerticles()<<" "<<getEdgesNumber()<<endl;
        for(i=0; i<N; ++i)
        {
            std::copy(polaczenia[i].begin(), polaczenia[i].end(), std::ostream_iterator<int>(ss, "\t"));
            ss<<endl;
        }
        return ss.str();
    }
};


bool compareEdges(const EDGE& e1, const EDGE& e2)
{

    return e1.second>e2.second;
}


namespace std
{
ostream& operator<<(ostream& cout, const list<EDGE>& es)
{
    std::copy(es.begin(), es.end(), std::ostream_iterator< EDGE >(cout, "\n"));
    return cout;
}

ostream& operator<<(ostream& cout, const Graph& G)
{
    cout<<(std::string)G;
    return cout;
}
}

typedef std::set<uint> DRZEWO;
typedef std::set<DRZEWO> LAS;

typedef pair<int, int> WierzchWaga;

ostream &operator<<(ostream &cout, const vector<int> &c)
{
    for(size_t i=0; i<c.size(); ++i)
        cout<<"["<<i<<"]="<<c[i]<<"  ";
    cout<<endl;
    return cout;
}


////////////////////////////////
//   DIJKSTRA START
////////////////////////////////
//define preprocessor variable HEAP to use heap instead of simple vector
//#define HEAP

struct CompareVerticles
{
    std::vector<int> &d;
    CompareVerticles(std::vector<int> &d):d(d)
    {
    }
    bool operator()(int a, int b) const
    {
        return d[a]<d[b];
    }
};

struct AddNeighbours: public unary_f
{
    vector<int> &Q;
    vector<int> &d;
    vector<int> &p;
    CompareVerticles compareVerticles;
    AddNeighbours(vector<int> &Q, vector<int> &d, CompareVerticles compare, vector<int> &p):compareVerticles(compare),Q(Q),d(d),p(p) {};
    //relaksacja wierzcholka jesli mozna
    bool operator()(const EDGE &a)
    {
        int fromVert=a.first.first;
        int toVert=a.first.second;
        int newPathLength=d[fromVert]+a.second;
        if(d[toVert]>newPathLength)
        {
            d[toVert]=newPathLength;
            Q.push_back(toVert);
#ifdef HEAP
            push_heap(Q.begin(), Q.end(), compareVerticles);
#endif
        }
        return true;
    }
};


vector<int> Dij(const Graph& G, int from)//minimal spanning tree
{
    std::cout<<"dijkstra starts\n";
    if(G.getEdgesNumber()<1)
    {
        MessageBoxA(0,"","graph must have positive number of edges",0);
        return vector<int>(0);
    }
    int N=G.getNumberOfVerticles();
    std::vector<int> p; //previouses
    std::vector<int> d(N,9999999);
    std::vector<int> Q; //heap
    Q.reserve(N);
    p.reserve(N);

    Q.push_back(from);
    //was[from]=true;
    d[from]=0;
    CompareVerticles compareVerticles(d);
    AddNeighbours addUNeighboursToQ(Q,d,compareVerticles,p);
    int u;
    while(Q.size()!=0)
    {
#ifdef HEAP
        u=Q.front();
#elif defined PRIOQUE
#else //simple vector
        std::vector<int>::iterator minit=std::min_element(Q.begin(), Q.end(), compareVerticles);
        u=*minit;
#endif
#ifdef HEAP
        pop_heap(Q.begin(), Q.end(), compareVerticles);
        Q.pop_back();
#else //simple vector
        std::swap(Q[minit-Q.begin()], Q.back());
        Q.pop_back();
#endif
        G.iterateThroughNeighbours(u,addUNeighboursToQ);
    }
    return d;
}

////////////////////////////////
//   DIJKSTRA END
////////////////////////////////



string strjoin(const char *algos[], int tablen, const char *separator)
{
    std::stringstream algorithms;
    std::copy(algos,algos+tablen, std::ostream_iterator<const char*>(algorithms, separator));
    std::string algs_str=algorithms.str();
    algs_str=algs_str.substr(0, algs_str.length()-strlen(separator));
    return algs_str;
}

void saveResult(const char *algo_choice, const char *impl_choice, int V, double E, int ticks)
{
    std::fstream err("C:\\Documents and Settings\\epsilon\\Pulpit\\sdizo\\release\\results.txt", std::fstream::out | std::fstream::app);
    char s='\t';
    err<<algo_choice<<s<<impl_choice<<s<<V<<s<<E<<s<<ticks<<endl;
    cout<<algo_choice<<" "<<impl_choice<<" "<<V<<" "<<E<<" "<<ticks<<"tcks="<<(ticks*1000/CLOCKS_PER_SEC)<<"ms"<<endl;
    err.close();
}
void saveGraph(const Graph &G)
{
    std::fstream graphOutput("c:\\lastgraph.txt", std::fstream::out | std::fstream::trunc);
    graphOutput<<G.getNumberOfVerticles()<<" "<<G.getEdgesNumber()<<endl;
    graphOutput<<G;
    graphOutput.close();
}

void doAlgorithm(const char *algo, const char *impl, const Graph &G)
{
    if(strcmp(algo,"dijkstra")==0)
    {
        int start=GetTickCount();
        vector<int> d=Dij(G,0);
        //proceedAlgorithm("dijkstra","list",G);
        int end=GetTickCount();
        //cout<<d<<endl;
        //cout<<G<<endl;
        saveResult("dijkstra","list", G.getNumberOfVerticles(), G.getEdgesNumber(), end-start);
    }
}


////////////////////////////////
// BELLMAN FORD START
////////////////////////////////
#define INFTY 9999999
struct Relaxation: public unary_f
{
    std::vector<uint> &d;
    Relaxation(std::vector<uint> &d):d(d) {}
    bool operator()(const EDGE &e)
    {
        int from=e.first.first;
        int to=e.first.second;
        int cost=e.second;
        if(d[from]<INFTY && d[to]>d[from]+cost) //relaxation is needed //DANGER: vulnerable to cycles.
        {
            d[to]=d[from]+cost;
            //p[to]=from;
        }
        return true;
    }
};
vector<uint> FB(Graph& G, int from)//minimal spanning tree
{

    //http://www.ibiblio.org/links/applets/appindex/graphtheory.html
    if(G.getNumberOfVerticles()<1)
    {
        MessageBoxA(0,"","graph must have positive number of verticles",0);
        return vector<uint>(0);
    }
    int N=G.getNumberOfVerticles();
    std::vector<uint> d(N, INFTY); //distance to infty (at least in 64-bit world ;) )

    Relaxation relaxIfNeeded(d);
    d[from]=0;

    for(int u=0; u<N; u++) //for each verticle
    {
        for(int i=0; i<N; i++) //for each verticle
            //if(1)	std::for_each(G.polaczenia[i].begin(), G.polaczenia[i].end(), relaxIfNeeded);
            G.iterateThroughNeighbours(i, relaxIfNeeded);
    }
    return d;
}
////////////////////////////////
// BELLMAN FORD END
////////////////////////////////


////////////////////////////////
// MST PRIME START
////////////////////////////////
bool compareEdgesPrime(const EDGE& e1, const EDGE& e2)
{
    return e1.second>e2.second;
}

struct AddAllNeighboursToLocalEdgesHeap: public unary_f
{
    std::vector<EDGE> &edgesHeap;
    std::vector<int> &MDR;
    AddAllNeighboursToLocalEdgesHeap(std::vector<int> &MDR, std::vector<EDGE> &edgesHeap):MDR(MDR),edgesHeap(edgesHeap) {}
    bool operator()(const EDGE &p)
    {
        if(MDR[p.first.first])
            return false; //zabezpieczenie przed cyklicznoscia
        edgesHeap.push_back(p);
        push_heap(edgesHeap.begin(), edgesHeap.end(), compareEdgesPrime);
        return true;
    }
};

Graph &Prime(const Graph& G, Graph &mst)//minimal spanning tree
{
    mst.clearEdgesList();
    if(G.getEdgesNumber()<1)
    {
        MessageBoxA(0,"","graph must have positive number of verticles",0);
        return mst;
    }
    mst.setNumberOfVerticles(G.getNumberOfVerticles());
    std::vector<int> MDR; //wierzcholki juz dodane
    std::vector<EDGE> edgesHeap;
    MDR.resize(G.getNumberOfVerticles());
    AddAllNeighboursToLocalEdgesHeap addAllNeighboursToLocalEdgesHeap(MDR,edgesHeap);
    bool doit=false;
    int i=0;
    do
    {
        doit=false;
        //krawedzie wychodzace z i-tego wierzcholka wrzucamy do Q (zbiornika krawedzi)
        //dodawanie krawedzi nowego wierzcholka do kopca krawedzi
        G.iterateThroughNeighbours(i,addAllNeighboursToLocalEdgesHeap);
        MDR[i]=true;	//wybieramy poczatkowy wierzcholek

        EDGE nearest;
again:
        doit=true;
        if(edgesHeap.empty()) break;
        nearest=*edgesHeap.begin();
        //std::cout<<((std::string)mst);
        pop_heap(edgesHeap.begin(), edgesHeap.end(), compareEdges);
        edgesHeap.pop_back();
        if(MDR[nearest.first.second] && MDR[nearest.first.first])
            goto again;
        else
            mst.connectNodes(nearest.first.first,nearest.first.second,nearest.second);

        i=nearest.first.second;
    }
    while (edgesHeap.size()!=0 || doit);

    return mst;
}
////////////////////////////////
// MST PRIME END
////////////////////////////////

////////////////////////////////
// MST KRUSKAL START
////////////////////////////////
typedef std::set<uint> DRZEWO;
typedef std::set<DRZEWO> LAS;

Graph &Kruskal(const Graph& G, Graph &mst)//minimal spanning tree
{
    if(G.getNumberOfVerticles()<1)
    {
        MessageBoxA(0,"","graph must have positive number of verticles",0);
        return mst;
    }
    mst.clearEdgesList();
    int N=G.getNumberOfVerticles();
    mst.setNumberOfVerticles(N);

    std::vector<EDGE> edgesHeap(G.edgesHeap); //copy od edges

    LAS L;

    for(int i=0; i<G.getNumberOfVerticles(); i++)
    {
        DRZEWO w;
        w.insert(i);
        L.insert(w);
    }

    make_heap(edgesHeap.begin(), edgesHeap.end(), compareEdges);

    do
    {
        if(edgesHeap.empty()) break;
        EDGE nearest=*edgesHeap.begin();
        DRZEWO firstTree,secondTree;
        for(LAS::iterator it= L.begin(); it!=L.end(); it++)
        {
            DRZEWO d=*it;
            if(d.find(nearest.first.first)!=d.end())
                firstTree=*it;
        }
        for(LAS::iterator it= L.begin(); it!=L.end(); it++)
        {
            DRZEWO d=*it;
            if(d.find(nearest.first.second)!=d.end())
                secondTree=*it;
        }
        if(firstTree!=secondTree) //jesli laczy dwa rozne drzewa
        {
            DRZEWO &f=firstTree;
            DRZEWO &s=secondTree;
            f.insert(s.begin(), s.end());
            for(LAS::iterator it=L.begin(); it!=L.end(); it++)
                if(*it==s)
                {
                    L.erase(it);
                    break;
                }
            mst.connectNodes(nearest.first.first, nearest.first.second, nearest.second);
        }
        pop_heap(edgesHeap.begin(), edgesHeap.end(), compareEdges);
        edgesHeap.pop_back();
    }
    while (edgesHeap.size()!=0);

    return mst;
}
////////////////////////////////
// MST KRUSKAL END
////////////////////////////////


int main(int argc, const char *argv[])
{
    const char *algos[]= {"dijkstra", "bellmanford", "prime", "kruskal"};
    const char *implementations[]= {"list", "matrix"};
    int V, maxWeight, minWeight;
    float E;
    const char *algo, *impl;
    algo="prime";
    impl="list";
    V=1000;
    E=.8; //wspolczyniik wypelnienia grafu pelnego
    maxWeight=100;
    minWeight=1;
    if(argc>=2) algo=argv[2-1];
    if(argc>=3) impl=argv[3-1];
    if(argc>=4)	V=atoi(argv[4-1]);
    if(argc>=5)	E=atof(argv[5-1]);
    if(argc>=6) maxWeight=atoi(argv[6-1]);
    if(argc>=7) minWeight=atoi(argv[7-1]);

    int ALGO=0;
    if(strcmp(algo,algos[0])==0) ALGO=1;
    else if(strcmp(algo,algos[1])==0) ALGO=2;
    else if(strcmp(algo,algos[2])==0) ALGO=3;
    else if(strcmp(algo,algos[3])==0) ALGO=4;

    if(argc>=5 || ALGO!=0)
    {
        Graph *G=NULL;
        Graph *mst=NULL;
        bool isList=(strcmp(impl,implementations[0])==0);
        if(isList)
            G=new GraphList(0);
        else //"matr"
            G=new GraphMatrix(0);
        G->generateGraph(V,E,maxWeight, minWeight);
        int t1=GetTickCount();
        switch(ALGO)
        {
        case 1:
            Dij(*G, 0);
            break;
        case 2:
            FB(*G, 0);
            break;
        case 3:
            if(isList)
                mst=new GraphList(0);
            else //"matr"
                mst=new GraphMatrix(0);
			std::cout<<"prime"<<endl;
            Prime(*G,*mst);
            delete mst;
            break;
        case 4:
            if(strcmp(impl,"list")==0)
                mst=new GraphList(0);
            else //"matr"
                mst=new GraphMatrix(0);
			std::cout<<"kruskal"<<endl;
            Kruskal(*G,*mst);
            delete mst;
            break;
        }
        int t2=GetTickCount();
        delete G;
        std::cout<<"took: "<<(t2-t1)<<endl;
        saveResult(algo, impl, V, E, t2-t1);
        ;
        //doAlgorithm("dijkstra","matrix",G);
    }
    else
    {
        printf("usage: prog [%s] [%s] number_of_verticles:int number_of_edges:int [maxWeight:int [minWeight:int]]"
               , strjoin(algos, 4, "|").c_str()
               , strjoin(implementations, 2, "|").c_str());
        printf("egg: %s %s %d %d\n\n", algos[0], implementations[0], 60, 3555);
        return 1;
    }
    return 0;
}
