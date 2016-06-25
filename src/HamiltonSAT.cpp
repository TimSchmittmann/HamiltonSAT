//============================================================================
// Name        : HamiltonSAT.cpp
// Author      : Tim Schmittmann
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>
#include <set>
#include <utility>
#include <map>
#include <bitset>
#include <math.h>

using namespace std;

bool debugging_enabled = true;

#define DEBUG(x) do { \
  if (debugging_enabled) { std::cerr << x << std::endl; } \
} while (0)

#define INFO(x) do { \
  std::cerr << x << std::endl; \
} while (0)


// You could also take an existing vector as a parameter.
vector<string> split(string str, char delimiter) {
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;

  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }

  return internal;
}


int main (int argc, char *argv[])
{
	string root = "../";
	ifstream infile(root+"graphs/hc-4.col");
	string line;

	set<pair<unsigned, unsigned>> edges;
	map<pair<unsigned, unsigned>, bool> mappedEdges;
	unsigned nodes;
	unsigned nr_of_edges;
	while (getline(infile, line))
	{
		vector<string> tokens = split(line, ' ');

		if (tokens[0][0] == 'e' && tokens.size() == 3) {
			pair<unsigned, unsigned> p = make_pair(stoul(tokens[1]), stoul(tokens[2]));
			edges.insert(p);
			mappedEdges.insert(make_pair(p, true));
		} else if(tokens[0][0] == 'p' && tokens.size() == 4) {
			nodes = stoul(tokens[2]);
			nr_of_edges = stoul(tokens[3]);
		}

	}

	vector<string> lines;
	unsigned clauses = 0;
	unsigned oldclauses = 0;
	unsigned circlelength = nodes+1;

	stringstream ss;

	ss << "c cond1" << endl;
	for(unsigned v = 1; v<=nodes; v++) {
		for(unsigned i = 1; i<=circlelength; i++) {
			ss << i << v << ' ';
		}
		ss << 0 << endl;
		clauses++;
	}
	ss << "c Clause diff " << clauses-oldclauses << endl;
	oldclauses = clauses;
	ss << "c Clauses " << clauses << endl;

	ss << "c cond2" << endl;
	for(unsigned v = 1; v<=nodes; v++) {
		unsigned i = 1;
		//Allow first and last path to contain same node
		for(unsigned j = 2; j<circlelength; j++) {
			ss << '-' << i << v << ' ' << '-' << j << v << ' ' << 0 << endl;
			clauses++;
		}
		for(i = 2; i<=circlelength-1; i++) {
			for(unsigned j = i+1; j<=circlelength; j++) {
				ss << '-' << i << v << ' ' << '-' << j << v << ' ' << 0 << endl;
				clauses++;
			}
		}
	}
	ss << "c Clause diff " << clauses-oldclauses << endl;
	oldclauses = clauses;
	ss << "c Clauses " << clauses << endl;

	ss << "c cond3" << endl;
	for(unsigned i = 1; i<=circlelength; i++) {
		for(unsigned j = 1; j<=nodes; j++) {
			ss << i << j << ' ';
		}
		ss << 0 << endl;
		clauses++;
	}
	ss << "c Clause diff " << clauses-oldclauses << endl;
	oldclauses = clauses;
	ss << "c Clauses " << clauses << endl;

	ss << "c cond4" << endl;
	for(unsigned i = 1; i<=circlelength; i++) {
		for(unsigned v = 1; v<=nodes-1; v++) {
			for(unsigned w = v+1; w<=nodes; w++) {
				ss << '-' << i << v << ' ' << '-' << i << w << ' ' << 0 << endl;
				clauses++;
			}
		}
	}
	ss << "c Clause diff " << clauses-oldclauses << endl;
	oldclauses = clauses;
	ss << "c Clauses " << clauses << endl;

	ss << "c cond5" << endl;
	for(unsigned v = 1; v<=nodes; v++) {
		for(unsigned w = 1; w<=nodes; w++) {
			if(!mappedEdges.count(make_pair(v,w))) {
				for(unsigned i = 1; i<=circlelength-1; i++) {
					ss << '-' << i << v << ' ' << '-' << i+1 << w << ' ' << 0 << endl;
					clauses++;
				}
			}
		}
	}
	ss << "c Clause diff " << clauses-oldclauses << endl;
	oldclauses = clauses;
	ss << "c Clauses " << clauses << endl;

	ss << "c cond6" << endl;
	for(unsigned i=0; i<pow(2, nodes); i++) {
		for(unsigned v=1; v<=nodes; v++) {
			ss << (((i >> (v-1)) & 1) ? circlelength : 1) << v << ' ';
		}
		ss << 0 << endl;
		clauses++;
	}

	ss << "c Clause diff " << clauses-oldclauses << endl;
	oldclauses = clauses;
	ss << "c Clauses " << clauses << endl;

	string filename = root+"tmp/hc-4-clauses.col";
	string resultname = root+"tmp/results.txt";
	ofstream outfile(filename);

//	outfile << "p cnf " << nodes*circlelength << ' ' << clauses << endl;
	outfile << ss.rdbuf();
	
	outfile.close();

	cout << ("minisat "+filename+" "+resultname).c_str();
	system(("minisat -verb=2 "+filename+" "+resultname).c_str());
	
//	for (set<pair<unsigned,unsigned>>::iterator it=edges.begin(); it!=edges.end(); ++it)
//		 DEBUG("From: " << it->first << " To: " << it->second);


	return 0;
}

