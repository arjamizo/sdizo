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

bool odd;

class Graph
{
public:
    //std::vector<Vertex > nodes;
    std::vector< list<EDGE> > polaczenia; //tablica list par opisujacych polaczenie [para polaczonych wierzcholkow, wag]

    std::vector<EDGE> edgesHeap;

    Graph (int N)
    {
        polaczenia.resize(N);
        //edgesHeap;
    }

    void connectNodes(uint from, uint to, uint weight)
    {
        EDGE k(PAIRINT(from, to),weight);
        polaczenia[from].push_back(k);
        edgesHeap.push_back(k);
        std::swap(from, to);
        polaczenia[from].push_back(EDGE(PAIRINT(from, to),weight));
    }

    operator std::string() const
    {
        std::stringstream ss;
        //ss<<"|G|="<<polaczenia.size()<<"\n";
        std::copy(edgesHeap.begin(), edgesHeap.end(), std::ostream_iterator< EDGE >(ss, "\n"));
        ss<<"";
        return ss.str();
    }
};

bool compareEdges(const EDGE& e1, const EDGE& e2)
{

    return e1.second>e2.second;
}


namespace std
{
ostream& operator<<(ostream& cout, const EDGE& e)
{
    cout<<
        //(::odd?"\t":"")<<
        e.first.first<<" "<<e.first.second<<" "<<e.second;
    return cout;
}
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

istream& operator>>(istream& cin, Graph& G)
{
    int x,y,w;
    for(int i=0; i<G.polaczenia.size(); ++i)
    {
        cin>>x;
        cin>>y;
        cin>>w;
        //cout<<"wczytano krawdz "<<x<<" to "<<y<<" for "<<w<<"\n";
        if(G.polaczenia.capacity()<std::max(x,y)+1)
            G.polaczenia.resize(std::max(x,y)+1);
        G.connectNodes((uint)x,(uint)y,(uint)w);
    }
    return cin;
}
}


Graph genFullGraph(int N)
{
    //G.polaczenia.resize(N);
    Graph G(N);
    srand(time(NULL));
    for(int x=0; x<N; ++x)
        for(int y=0; y<N; ++y)
        {
            if (x<=y)
                continue;
            G.connectNodes(x,y, rand()%100+1);
        }
    return G;
}


string czasWykonania(int tstart)
{
    int tend=GetTickCount();
    int s,m,h,czas;
    czas=tend-tstart;
    s = czas / 1000; // reszta z dzielenia ilosci sekund przez 60 daje nam "koncowke"
    m = (s / 60) % 60; // dzielimy przez 60, aby uzyskac ilosc minut, ale interesuje nas tylko reszta z dzielenia przez 60, aby przy czasie >1h nie otrzymac dziwnych wynikow
    h = s / 3600; // dzielimy przez ilosc sekund w godzinie i mamy ilosc godzin
    s = (czas / 1000) - (60 * m); // reszta z dzielenia ilosci sekund przez 60 daje nam "koncowke"
    stringstream ss;
    ss<<"czas wykonania"<< h << ":" << m << ":" << s<<"."<<(czas%1000);
    return ss.str();
}

typedef std::set<uint> DRZEWO;
typedef std::set<DRZEWO> LAS;

typedef pair<int, int> WierzchWaga;


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

struct AddNeighbours
{
    vector<int> &Q;
    vector<int> &d;
    vector<int> &p;
    CompareVerticles compareVerticles;
    AddNeighbours(vector<int> &Q, vector<int> &d, CompareVerticles compare, vector<int> &p):compareVerticles(compare),Q(Q),d(d),p(p) {};
    void operator()(const EDGE &a)
    {
		int fromVert=a.first.first;
		int toVert=a.first.second;
		int newPathLength=d[fromVert]+a.second;
		if(d[toVert]>newPathLength)
		{
			d[toVert]=newPathLength;
			Q.push_back(toVert);
			push_heap(Q.begin(), Q.end(), compareVerticles);
		}
    }
};

//const in Graph only for temporary argumenting
vector<int> Dij(const Graph& G, int from)//minimal spanning tree
{
	#define HEAP
    if(G.polaczenia.size()<1)
    {
        MessageBoxA(0,"","graph must have positive number of verticles",0);
        return vector<int>(0);
    }
    Graph dij(G.polaczenia.size());
    std::vector<int> Q; //heap
    Q.reserve(G.polaczenia.size());
    std::vector<int> p; //previouses
    p.reserve(G.polaczenia.size());
    std::vector<int> d(G.polaczenia.size(),9999999); //distance
    //std::vector<bool> was(G.polaczenia.size(), false);
    //bool *was=new bool[G.polaczenia.size()];//were we in this verticle (this vector is used istead of infinity value in d)
    //std::fill(was, was+G.polaczenia.size(), false);

    Q.push_back(from);
    //was[from]=true;
    d[from]=0;
	CompareVerticles compareVerticles(d);
	AddNeighbours addUNeighboursToQ(Q,d,compareVerticles,p);
    while(Q.size()!=0)
    {
    	#ifdef HEAP
        int u=Q.front();
        #elif defined PRIOQUE
        #else //simple vector
        int u=std::min_element(Q.begin(), Q.end(), compareVerticles);
        #endif
        #ifdef HEAP
        pop_heap(Q.begin(), Q.end(), compareVerticles);
        Q.pop_back();
        #else //simple vector

        #endif
        if(1) for_each(G.polaczenia[u].begin(), G.polaczenia[u].end(), addUNeighboursToQ);
        else for(list<EDGE>::const_iterator it=G.polaczenia[u].begin(); it!=G.polaczenia[u].end(); it++)
        {
        	EDGE a=*it;
			int fromVert=a.first.first;
			int toVert=a.first.second;
			int newPathLength=d[fromVert]+a.second;
			if(d[toVert]>newPathLength)
			{
				d[toVert]=newPathLength;
				Q.push_back(toVert);
				push_heap(Q.begin(), Q.end(), compareVerticles);
			}
		}
    }
    return d;
    #undef HEAP
}

ostream &operator<<(ostream &cout, const vector<int> &c)
{
	for(int i=0; i<c.size(); ++i)
		cout<<"["<<i<<"]="<<c[i]<<"  ";
	cout<<endl;
	return cout;
}

void testGraph(const Graph& G, int edges=0)
{
    stringstream ss;
    ss<<((uint)G.polaczenia.size())<<".txt";
    std::cout<<ss.str();
    string output=ss.str();
    int tstart=GetTickCount();
    vector<int> res=Dij(G,0);
    //cout<<res;
    //cout<<((std::string)res);
    //cout<<czasWykonania(tstart)<<endl;
    std::fstream err ("..\\wyniki_dijkstra.txt", fstream::in | fstream::out| fstream::app);
    err<<G.polaczenia.size()<<"\t"<<edges<<"\t"<<(GetTickCount()-tstart)<<"\n";
    err.close();
}

struct BelowFilter {
	int lim;
	BelowFilter(int lim):lim(lim){}
	bool operator()(int n)
	{
		return n<=lim;
	}
};
struct EqualFilter {
	int lim;
	EqualFilter(int lim):lim(lim){}
	bool operator()(int n)
	{
		return n==lim;
	}
};

Graph generateGraph(int v, int e, int weightMax=100, int weightMin=1)
{
	Graph G(v);
	vector<int> a,b;
	vector<int> cachedA(v, 0);
	srand(time(0));
	a.reserve(v);
	b.reserve(v);
	for(int i=0; i<v; ++i)
	{
		a.push_back(i);
		b.push_back(i);
	}
	for(int i=0; i<e; ++i)
	{
		std::random_shuffle(a.begin(), a.end());
		int src=a.back();
		if(++cachedA[src]==v)
		{
			a.pop_back();
			list<int> copyB(b.begin(), b.end());
			EqualFilter equalFilter(src);
			remove_if(copyB.begin(), copyB.end(), equalFilter);
			b=std::vector<int>(copyB.begin(), copyB.end());
		}
		vector<int> copyOfB(b);
		BelowFilter belowFilter(src);
		std::remove_if(copyOfB.begin(), copyOfB.end(), belowFilter);
		std::random_shuffle(copyOfB.begin(), copyOfB.end());
		int dest=copyOfB.front();
		G.connectNodes(src,dest,rand()%(weightMax-weightMin)+weightMin);
	}
	return G;
}

string strjoin(const char *algos[], int tablen, const char *separator)
{
	std::stringstream algorithms;
	std::copy(algos,algos+tablen, std::ostream_iterator<const char*>(algorithms, separator));
	std::string algs_str=algorithms.str();
	algs_str=algs_str.substr(0, algs_str.length()-strlen(separator));
	return algs_str;
}

int main(int argc, const char *argv[])
{
	if(1)
	{
		const char *algos[]={"dijkstra", "bellmanford", "kruskal", "prime"};
		const char *implementations[]={"list", "matrix"};

		int V=atoi("600");
		int E=atoi("3600");
		const char *algo_choice="dijkstra";
		const char *impl_choice="list";

		int maxWeight=atoi("100");
		int minWeight=atoi("1");
		if(argc<5)
		{
			printf("usage: prog [%s] [%s] number_of_verticles:int number_of_edges:int [maxWeight:int [minWeight:int]]"
					, strjoin(algos, 4, "|").c_str()
					, strjoin(implementations, 2, "|").c_str());
			printf("running with default arguments %s %s %d %d\n\n", algo_choice, impl_choice, V, E);
			if(argc>=6) maxWeight=atoi(argv[6-1]);
			if(argc>=7) minWeight=atoi(argv[7-1]);
		}
		else
		{
			V=atoi(argv[3]);
			E=atoi(argv[4]);
			algo_choice=argv[1];
			impl_choice=argv[2];
		}
		int start, end;
		Graph G(generateGraph(V,E,maxWeight, minWeight));
		cout<<"Graph has been generated\n";
		if(strcmp(algo_choice,algos[0])==0)
			{
				start=GetTickCount();
				Dij(G, 0);
				end=GetTickCount();
			}
		else
		{
			goto deadbeef;
		}
		{
			int ticks=end-start;
			std::cout<<algo_choice<<" "<<impl_choice<<" "<<V<<" "<<E<<" "<<ticks<<endl;
			std::fstream err("results.txt", std::fstream::out | std::fstream::app);
			err<<algo_choice<<" "<<impl_choice<<" "<<V<<" "<<E<<" "<<ticks<<endl;
			err.close();
		}
		return 0;
		deadbeef:
		printf("not supported yet.\n0xDEADBEEF");
	}
    else if(1)
    {
        int N=10;
        Graph G(N);
        if(argc>1)
            N=atoi(argv[1]);
        for(int i=100; true || i<4000; i+=100)
        {
            G=genFullGraph(i);
            //	std::cout<<(string)G;
            testGraph(generateGraph(i, i/4*i));
        }
        return 0;
    }

    std::fstream in ("..\\graph.txt", fstream::in | fstream::out);

    int N, M;
    in>>N>>M;
    //cout<<N<<" "<<M<<endl;
    Graph G(N);
    G.polaczenia.resize(N);
    in>>G;
    cout<<Dij(G,0);
    in.close();
    system("PAUSE");
}
