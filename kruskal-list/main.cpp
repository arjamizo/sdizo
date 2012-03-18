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
		G.connectNodes(x,y, rand()%100);
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

Graph Kruskal(Graph& G)//minimal spanning tree
{
	if(G.polaczenia.size()<1)
	{
		MessageBoxA(0,"","graph must have positive number of verticles",0);
		return Graph(0);
	}

	std::vector<EDGE> edgesHeap(G.edgesHeap);

	Graph Kruskal(G.polaczenia.size());

	LAS L;
	//std::vector<DRZEWO&> wKtorymDrzewieWierzcholek;

	for(int i=0; i<G.polaczenia.size(); i++)
		{
			DRZEWO w;
			w.insert(i);
			L.insert(w);
			//wKtorymDrzewieWierzcholek.push_back(&w);
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
				Kruskal.connectNodes(nearest.first.first, nearest.first.second, nearest.second);
			}
		pop_heap(edgesHeap.begin(), edgesHeap.end(), compareEdges);
		edgesHeap.pop_back();
	}
	while (edgesHeap.size()!=0);

	return Kruskal;
}


void testGraph(Graph& G)
{
	stringstream ss;
	ss<<((uint)G.polaczenia.size())<<".txt";
	std::cout<<ss.str();
	string output=ss.str();
	int tstart=GetTickCount();
	Graph res=Kruskal(G);
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
	cout<<Kruskal(G);
	in.close();
}
