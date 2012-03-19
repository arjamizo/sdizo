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
        if(G.polaczenia.size()<std::max(x,y)+1)
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
    void operator()(EDGE &a)
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

vector<int> Dij(Graph& G, int from)//minimal spanning tree
{
    if(G.polaczenia.size()<1)
    {
        MessageBoxA(0,"","graph must have positive number of verticles",0);
        return vector<int>(0);
    }
    Graph dij(G.polaczenia.size());
    std::vector<int> Q; //heap
    std::vector<int> p; //previouses
    std::vector<int> d(G.polaczenia.size(),9999999); //distance
    //std::vector<bool> was(G.polaczenia.size(), false);
    //bool *was=new bool[G.polaczenia.size()];//were we in this verticle (this vector is used istead of infinity value in d)
    //std::fill(was, was+G.polaczenia.size(), false);

    Q.push_back(from);
    //was[from]=true;
    d[from]=0;
    while(Q.size()!=0)
    {
        int u=Q.front();
        CompareVerticles compareVerticles(d);
        pop_heap(Q.begin(), Q.end(), compareVerticles);
        Q.pop_back();
        AddNeighbours addUNeighboursToQ(Q,d,compareVerticles,p);
        if(0) for_each(G.polaczenia[u].begin(), G.polaczenia[u].end(), addUNeighboursToQ);
        else for(list<EDGE>::iterator it=G.polaczenia[u].begin(); it!=G.polaczenia[u].end(); it++)
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

ostream &operator<<(ostream &cout, const vector<int> &c)
{
	for(int i=0; i<c.size(); ++i)
		cout<<"["<<i<<"]="<<c[i]<<"  ";
	cout<<endl;
	return cout;
}

void testGraph(Graph& G)
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
    std::fstream err ("wyniki.txt", fstream::in | fstream::out| fstream::app);
    err<<output<<"\t"<<(GetTickCount()-tstart)<<"\n";
    err.close();
}

int main(int argc, const char *argv[])
{
    if(true)
    {
        int N=10;
        Graph G(N);
        if(argc>1)
            N=atoi(argv[1]);
        for(int i=100; i<4000; i+=100)
        {
            G=genFullGraph(i);
            //	std::cout<<(string)G;
            testGraph(G);
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
}
