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


template <class InputIterator, class OutputIterator, class Predicate>
OutputIterator copy_if(InputIterator first, InputIterator last,
                       OutputIterator result, Predicate pred)
{
    while(first!=last)
    {
        if(pred(*first))
            *result++ = *first;
        ++first;
    }
    return result;
};



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

bool odd;

struct BelowFilter
{
    int lim;
    BelowFilter(int lim):lim(lim) {}
    bool operator()(int n)
    {
        return n<=lim;
    }
};
struct EqualFilter
{
    int lim;
    EqualFilter(int lim):lim(lim) {}
    bool operator()(int n)
    {
        return n==lim;
    }
};

struct unary_f
{
	virtual bool operator()(const EDGE&) =0;
};

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
    }
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

        Graph &ths=*this;
        std::random_shuffle(omega.begin(),omega.end());
        int cnt=0;
        int e=fill*omega.size();
        for(int o=0; o<omega.size(); ++o)
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
		int number=getEdgesNumber();
		cnt=cnt;
		//std::cout<<*this;
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
        int v=getEdgesNumber();
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
    ::odd=!::odd;
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
    for(int i=0; i<c.size(); ++i)
        cout<<"["<<i<<"]="<<c[i]<<"  ";
    cout<<endl;
    return cout;
}



//#define HEAP
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

int main(int argc, const char *argv[])
{
    int V, maxWeight, minWeight;
    float E;
    const char *algo, *impl;
    algo="dijkstra";
    impl="list";
    V=400;
    E=.8; //wspolczyniik wypelnienia grafu pelnego
    maxWeight=100;
    minWeight=1;
    if(argc>=2) algo=argv[2-1];
    if(argc>=3) impl=argv[3-1];
    if(argc>=4)	V=atoi(argv[4-1]);
    if(argc>=5)	E=atof(argv[5-1]);
    if(argc>=6) maxWeight=atoi(argv[6-1]);
    if(argc>=7) minWeight=atoi(argv[7-1]);

    if(1)
    {
    	Graph *G;
    	if(strcmp(impl,"list")==0)
			G=new GraphList(0);
    	else
			G=new GraphMatrix(0);
    	G->generateGraph(V,E,maxWeight, minWeight);
    	int t1=GetTickCount();
    	if(strcmp(algo,"dijkstra")==0) Dij(*G, 0);
    	int t2=GetTickCount();
    	std::cout<<"took: "<<(t2-t1)<<endl;
    	saveResult(algo, impl, V, E, t2-t1);
    	;
        //doAlgorithm("dijkstra","matrix",G);
    }
    else if(argc==2 && strcmp(argv[1],"repeat"))
    {
		GraphList G(0);
        G.loadGraphFromFile("c:\\lastgraph.txt");
        //int E=G.getEdgesNumber();
        //int V=G.polaczenia.size();
        //cout<<Dij(G,0);
        doAlgorithm("dijkstra","list",G);
    }
    else if(strcmp("dijkstra",algo)==0 && strcmp("list",impl)==0)
    {
        printf("interpreted arguments as %d %f %d %d\n", V, E, maxWeight, minWeight);

		GraphList G(0);
        G.generateGraph(V,E);
        printf("> graph size=%d,%d,eheap=%d\n",G.getNumberOfVerticles(),G.getEdgesNumber(), G.edgesHeap.size());
        doAlgorithm("dijkstra","list",G);
        //saveGraph(G);
        return 0;
    }
    else if(strcmp("dijkstra",algo)==0 && strcmp("matrix",impl)==0)
    {
        printf("interpreted arguments as %d %f %d %d\n", V, E, maxWeight, minWeight);

		GraphMatrix G(0);
        G.generateGraph(V,E);
        printf("> graph size=%d,%d,eheap=%d\n",G.getNumberOfVerticles(),G.getEdgesNumber(), G.edgesHeap.size());
        doAlgorithm("dijkstra","matrix",G);
        //saveGraph(G);
        return 0;
    }

    else if(argc==2)
    {
		GraphList G(0);
        G.loadGraphFromFile(argv[1]);
        doAlgorithm("dijkstra","list",G);
    }
    else
    {
        const char *algos[]= {"dijkstra", "bellmanford", "kruskal", "prime"};
        const char *implementations[]= {"list", "matrix"};
        printf("usage: prog [%s] [%s] number_of_verticles:int number_of_edges:int [maxWeight:int [minWeight:int]]"
               , strjoin(algos, 4, "|").c_str()
               , strjoin(implementations, 2, "|").c_str());
        printf("egg: %s %s %d %d\n\n", algos[0], implementations[0], 60, 3555);
        return 1;
    }
    return 0;
}
