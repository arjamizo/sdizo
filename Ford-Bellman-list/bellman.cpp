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

typedef unsigned long long uint;
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

#define INFTY 9999999
struct Relaxation
{
    std::vector<uint> &d;
    Relaxation(std::vector<uint> &d):d(d) {}
    void operator()(const EDGE &e)
    {
        int from=e.first.first;
        int to=e.first.second;
        int cost=e.second;
        if(d[from]<INFTY && d[to]>d[from]+cost) //relaxation is needed //DANGER: vulnerable to cycles.
        {
            d[to]=d[from]+cost;
            //p[to]=from;
        }
    }
};
vector<uint> FB(Graph& G, int from)//minimal spanning tree
{

    //http://www.ibiblio.org/links/applets/appindex/graphtheory.html
    if(G.polaczenia.size()<1)
    {
        MessageBoxA(0,"","graph must have positive number of verticles",0);
        return vector<uint>(0);
    }
    std::vector<uint> d(G.polaczenia.size(), INFTY); //distance to infty (at least in 64-bit world ;) )
    //std::copy(d.begin(), d.end(), std::ostream_iterator<uint>(std::cout, "\n"));


    Relaxation relaxIfNeeded(d);
    d[from]=0;

    int N=G.polaczenia.size();
	int cnt=0;
    for(int u=0; u<N; u++) //for each verticle
    {
		for(int i=0; i<N; i++) //for each verticle
        if(1)	std::for_each(G.polaczenia[i].begin(), G.polaczenia[i].end(), relaxIfNeeded);
        else
        {

            for(list<EDGE>::iterator it=G.polaczenia[i].begin(); it!=G.polaczenia[i].end(); it++)
                //relaxIfNeeded(*it);
            {
            	cnt++;
                EDGE e=*it;
                int from=e.first.first;
                int to=e.first.second;
                int cost=e.second;
                if(d[to]>d[from]+cost) //relaxation is needed //DANGER: vulnerable to cycles.
                {
                    d[to]=d[from]+cost;
                    //p[to]=from;
                }
            }
        }
    }
    return d;
}

template <class T> ostream &operator<<(ostream &cout, const vector<T> &c)
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
    vector<uint> res=FB(G,0);
    //cout<<res;
    //cout<<((std::string)res);
    //cout<<czasWykonania(tstart)<<endl;
    std::fstream err ("..\\wyniki_bellman.txt", fstream::in | fstream::out| fstream::app);
    err<<output<<"\t"<<(GetTickCount()-tstart)<<"\n";
    err.close();
}

int main(int argc, const char *argv[])
{
    if(1)
    {
        int N=10;
        Graph G(N);
        if(argc>1)
            N=atoi(argv[1]);
        for(int i=100; true || i<4000; i+=100)
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
    cout<<FB(G,0);
    in.close();
    //system("PAUSE");
}
