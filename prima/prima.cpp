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

#include <windows.h>

using namespace std;


	typedef pair<int, int> PAIRINT;
	typedef pair<PAIRINT, int> EDGE;

bool odd;

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
}

class Graph
{
	public:
	//std::vector<Vertex > nodes;
	std::vector< list<EDGE> > polaczenia; //tablica list par opisujacych polaczenie [para polaczonych wierzcholkow, wag]


	void connectNodes(int from, int to, int weight)
	{
		polaczenia[from].push_back(EDGE(PAIRINT(from, to),weight));
		std::swap(from, to);
		polaczenia[from].push_back(EDGE(PAIRINT(from, to),weight));
	}

	operator std::string()
	{
		std::stringstream ss;
		//ss<<"|G|="<<polaczenia.size()<<"\n";
		std::copy(polaczenia.begin(), polaczenia.end(), std::ostream_iterator< list<EDGE> >(ss, ""));
		ss<<"\n";
		return ss.str();
	}
};

bool compareEdges(const EDGE& e1, const EDGE& e2)
{

	return e1.second>e2.second;
}

Graph MST(Graph& G)//minimal spanning tree
{
	std::vector<int> MDR; //wierzcholki juz dodane
	std::vector<EDGE> edgesHeap;
	static Graph mst;
	MDR.resize(G.polaczenia.size());
	mst.polaczenia.resize(G.polaczenia.size());

	int i=0;
	do
	{
		MDR[i]=true;	//wybieramy poczatkowy wierzcholek
		//krawedzie wychodzace z i-tego wierzcholka wrzucamy na stos

		//dodawanie krawedzi nowego wierzcholka do kopca krawedzi
		for(std::list<EDGE>::iterator it=G.polaczenia[i].begin(); it!=G.polaczenia[i].end(); it++)
		{
			if(MDR[it->first.second])
				continue; //zabezpieczenie przed cyklicznoscia
			edgesHeap.push_back(*it);
			push_heap(edgesHeap.begin(), edgesHeap.end(), compareEdges);
		}

		EDGE nearest;
		again:
		if(edgesHeap.empty()) break;
		nearest=*edgesHeap.begin();
		//std::cout<<((std::string)mst);
		pop_heap(edgesHeap.begin(), edgesHeap.end(), compareEdges);
		edgesHeap.pop_back();
		if(MDR[nearest.first.second] && MDR[nearest.first.first])
			goto again;
		else
			mst.polaczenia[nearest.first.first].push_back(nearest);

		i=nearest.first.second;
	}
	while (edgesHeap.size()!=0);

	return mst;
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

int main(int argc, const char *argv[])
{
	Graph G;
	//Vertex v;
	//std::cout<<v;

	string input="0999F.txt";
	//string output="";
	//std::cout<<"argc="<<argc<<endl;
	if(argc>1)
	{
		input=argv[1];
	}
		string output=input;
		output.append(".ans");
		std::cout<<"wyjscie: "<<output<<"\n";
		std::fstream in (input.c_str(), fstream::in | fstream::out);
	//	std::fstream out (output.c_str(), fstream::in | fstream::out);
		std::fstream err ("wyniki.txt", fstream::in | fstream::out| fstream::app);

	int N,x,y,w,M;
	in>>N>>M; //N liczba wierzcholkow, M liczba polaczen
	//cout<<"wczytano graf o "<<M<<" krawedziach\n";
	for(int i=0; i<M; ++i)
	{
		in>>x;
		in>>y;
		in>>w;
		//cout<<"wczytano krawdz "<<x<<" to "<<y<<" for "<<w<<"\n";
		if(G.polaczenia.size()<std::max(x,y)+1)
			G.polaczenia.resize(std::max(x,y)+1);
		G.connectNodes(x,y,w);
	}
	in.close();

	//cout<<((std::string)G);

	int tstart=GetTickCount();
	cout<<((std::string)MST(G));
	cout<<czasWykonania(tstart)<<endl;
	err<<input<<"\t"<<(GetTickCount()-tstart)<<"\n";

	//std::cout<<pair<int,int>(1,2);
	//G.printNodes();
	//std::cout<<(std::string)G;
	return 0x0;
}
