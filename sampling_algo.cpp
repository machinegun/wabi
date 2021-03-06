//============================================================================
// Name        : Assembly_Genome.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
#include <ctime>
#include <fstream>
#include <string>
#include <numeric>
#include <map>

#include <NetworKit/graph/Graph.h>
#include <NetworKit/io/GMLGraphReader.h>
#include <NetworKit/centrality/ApproxBetweenness.h>
#include <NetworKit/centrality/Betweenness.h>
#include <NetworKit/centrality/ApproxBetweenness2.h>

using namespace std;
using namespace NetworKit;

int main(int argc, char* argv[]) {

	string filepath;
	int filterparam;
	int sampleNodes;
	filepath = argv[1];
	filterparam = atoi(argv[2]);
	sampleNodes = atoi(argv[3]);
	string repeat_file = argv[4];
	string rt_file = argv[5];
	clock_t start = clock();
	GMLGraphReader reader;
	Graph g = reader.read(argv[1]);
	ifstream ifile;
	ifile.open(argv[1]);
	string str;
	map<string,string> labelmap;
	string id;
	string label;
	while (std::getline(ifile, str)) {
		if(str.find("id") != string::npos){
			id = str.substr(7);
		}
		if(str.find("label") != string::npos) {
			label = str.substr(10);
			label.erase(0,1);
			label.erase(label.length()-1);
			labelmap[id] = label;
		}
	}

	cerr << "Nodes: " << g.nodes().size() << endl;
	cerr << "Edges: " << g.edges().size() << endl;
	double delta = 0.1;
	double epsilon = 0.1;
	
	ApproxBetweenness2 centrality(g,sampleNodes,false);
	centrality.run();
	ofstream myfile;
	filepath = filepath.substr(0,filepath.length()-4);
	myfile.open(repeat_file);
	vector<pair<node, double>> nodeRanks(g.nodes().size());
	nodeRanks = centrality.ranking();
	vector<double> scores = centrality.scores();
	double sum = std::accumulate(scores.begin(), scores.end(), 0.0);
	double mean = sum / scores.size();

	double sq_sum = std::inner_product(scores.begin(), scores.end(), scores.begin(), 0.0);
	double stdev = std::sqrt(sq_sum / scores.size() - mean * mean);
	cerr<<"Mean: "<<mean<<endl;
	cerr<<"Std Dev: "<<stdev<<endl;
	cerr<<"bound: "<<mean + filterparam*stdev<<endl;
	double bound = mean + filterparam*stdev;

	ofstream runfile;
        runfile.open(rt_file);
        runfile << (double) (clock() - start) / CLOCKS_PER_SEC << endl;

	for (int i = 0; i < g.nodes().size(); i++) {
		int isrepeat = 0;
		if(nodeRanks.at(i).second >= bound){
			isrepeat = 1;
		}
		string String = static_cast<ostringstream*>( &(ostringstream() << nodeRanks.at(i).first) )->str();
		myfile << nodeRanks.at(i).first <<"\t"<< nodeRanks.at(i).second<<"\t"<<labelmap[String]<<"\t"<<isrepeat<< endl;
	}

	return 0;
}

