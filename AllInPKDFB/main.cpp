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

namespace std {
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

class Graph
{
public:

    std::vector<EDGE> edgesHeap;
    int M;


	    //std::vector<Vertex > nodes;
    std::vector< list<EDGE> > polaczenia; //tablica list par opisujacych polaczenie [para polaczonych wierzcholkow, wag]

    Graph (int N, int M=0):M(M)
    {
        polaczenia.resize(N);
    }

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
    	int sum=0;
    	for(int i=0; i<polaczenia.size(); ++i)
    	{
			for(std::list<EDGE >::const_iterator it=polaczenia[i].begin(); it!=polaczenia[i].end(); ++it)
				//if(it->first.first<=it->first.second)
					++sum;
    	}
		return sum;
    }

    operator std::string() const
    {
        std::stringstream ss;
        //ss<<"|G|="<<polaczenia.size()<<"\n";
        string f;
        int i;
        int I=edgesHeap.size();
        for(i=0; i<I; ++i)
        {
			ss<<edgesHeap[i]<<endl;
			f=ss.str();
        }
        ss<<"";
        return ss.str();
    }
};

class GraphList: public Graph
{
	    //std::vector<Vertex > nodes;

    //iterates through all neighbours of u with function fnc
    virtual void iterateThroughNeighbours(int u, void (*fnc)(EDGE &e))=0;
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

istream& operator>>(istream& cin, Graph& G)
{
    int x,y,w;
    int i=0;
    int I=G.M;
    for(i=0; i<I; ++i)
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
			#ifdef HEAP
			push_heap(Q.begin(), Q.end(), compareVerticles);
			#endif
		}
    }
};

	//#define HEAP
vector<int> Dij(const Graph& G, int from)//minimal spanning tree
{
	std::cout<<"dijkstra starts\n";
    if(G.polaczenia.size()<1)
    {
        MessageBoxA(0,"","graph must have positive number of verticles",0);
        return vector<int>(0);
    }
    Graph dij(G.polaczenia.size());
    std::vector<int> p; //previouses
    std::vector<int> d(G.polaczenia.size(),9999999);
    std::vector<int> Q; //heap
    Q.reserve(G.polaczenia.size());
    p.reserve(G.polaczenia.size());

    Q.push_back(from);
    //was[from]=true;
    d[from]=0;
	const CompareVerticles compareVerticles(d);
	const AddNeighbours addUNeighboursToQ(Q,d,compareVerticles,p);
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
        if(1) for_each(G.polaczenia[u].begin(), G.polaczenia[u].end(), addUNeighboursToQ);
        else
			for(list<EDGE>::const_iterator it=G.polaczenia[u].begin(); it!=G.polaczenia[u].end(); it++)
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

Graph generateGraph(int v, double fill, int weightMax=100, int weightMin=1)
{
	Graph G(v);
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
	for(int o=0; o<omega.size(); ++o)
	{
		if(isIn[omega[o].first]==false || isIn[omega[o].second]==false)
		{
			isIn[omega[o].first]=true;
			isIn[omega[o].second]=true;
			G.connectNodes(omega[o].first, omega[o].second, rand()%(weightMax-weightMin)+weightMin);
			//swap(omega[o],omega[omega.size()-1]);
			//omega.pop_back();
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
		G.connectNodes(edge.first, edge.second, rand()%(weightMax-weightMin)+weightMin);
	}
	if(cnt<e)
		std::cerr<<"not enough edges\n";

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

Graph loadGraphFromFile(const char *fname)
{
		std::fstream in (fname, fstream::in | fstream::out);
		if(!in.is_open())
			{
				std::cerr<<"file could not be found!\n";
				return Graph(0);
			}
		int N, M;
		in>>N>>M;
		//cout<<N<<" "<<M<<endl;
		Graph G(N,M);
		G.polaczenia.resize(N);
		in>>G;
		in.close();
		return G;
}


void saveResult(const char *algo_choice, const char *impl_choice, int V, int E, int ticks)
{
	std::fstream err("C:\\Documents and Settings\\epsilon\\Pulpit\\sdizo\\results.txt", std::fstream::out | std::fstream::app);
	err<<algo_choice<<" "<<impl_choice<<" "<<V<<" "<<E<<" "<<ticks<<endl;
	cout<<algo_choice<<" "<<impl_choice<<" "<<V<<" "<<E<<" "<<ticks<<"tcks="<<(ticks*1000/CLOCKS_PER_SEC)<<"ms"<<endl;
	err.close();
}
void saveGraph(const Graph &G)
{
		std::fstream graphOutput("c:\\lastgraph.txt", std::fstream::out | std::fstream::trunc);
		graphOutput<<G.polaczenia.size()<<" "<<G.getEdgesNumber()<<endl;
		graphOutput<<G;
		graphOutput.close();
}

void doAlgorithm(const char *algo, const Graph &G)
{
	if(strcmp(algo,"dijkstra")==0)
	{
		int start=GetTickCount();
		vector<int> d=Dij(G,0);
		//proceedAlgorithm("dijkstra","list",G);
		int end=GetTickCount();
		//cout<<d<<endl;
		//cout<<G<<endl;
		saveResult("dijkstra","list", G.polaczenia.size(), G.getEdgesNumber(), end-start);
	}
}

int main(int argc, const char *argv[])
{
	Graph G(0);

	int V, maxWeight, minWeight;
	float E;
	const char *algo, *impl;
	algo="dijkstra";
	impl="list";
	V=3000;
	E=0.8;
	maxWeight=100;
	minWeight=1;
	if(argc>=2) algo=argv[2-1];
	if(argc>=3) impl=argv[3-1];
	if(argc>=4)	V=atoi(argv[4-1]);
	if(argc>=5)	E=atof(argv[5-1]);
	if(argc>=6) maxWeight=atoi(argv[6-1]);
	if(argc>=7) minWeight=atoi(argv[7-1]);

	if(argc==2 && strcmp(argv[1],"repeat"))
		{
			G=loadGraphFromFile("c:\\lastgraph.txt");
			int E=G.getEdgesNumber();
			int V=G.polaczenia.size();
			//cout<<Dij(G,0);
			doAlgorithm("dijkstra",G);
		}
	else if(strcmp("dijkstra",algo)==0 && strcmp("list",impl)==0)
	{
		printf("interpreted arguments as %d %f %d %d\n", V, E, maxWeight, minWeight);

		G=generateGraph(V,E);
		doAlgorithm("dijkstra",G);
		//saveGraph(G);
		return 0;
	}

	else if(argc==2)
	{
		G=loadGraphFromFile(argv[1]);
		doAlgorithm("dijkstra",G);
	}
	else
	{
		const char *algos[]={"dijkstra", "bellmanford", "kruskal", "prime"};
		const char *implementations[]={"list", "matrix"};
		printf("usage: prog [%s] [%s] number_of_verticles:int number_of_edges:int [maxWeight:int [minWeight:int]]"
				, strjoin(algos, 4, "|").c_str()
				, strjoin(implementations, 2, "|").c_str());
		printf("egg: %s %s %d %d\n\n", algos[0], implementations[0], 60, 3555);
		return 1;
	}
	return 0;
}
