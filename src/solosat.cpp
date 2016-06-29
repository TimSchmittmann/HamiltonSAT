//============================================================================
// Name        : HamiltonSAT.cpp
// Author      : Tim Schmittmann
// Version     :
// Copyright   : Your copyright notice
//============================================================================

#include <iostream>
#include <sstream>
#include <string.h>
#include <vector>
#include <set>
#include <utility>
#include <map>
#include <bitset>
#include <math.h>
#include <ctime>
#include <chrono>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include "boost/filesystem.hpp"
#undef BOOST_NO_CXX11_SCOPED_ENUMS

using namespace boost::filesystem;
using namespace std::chrono;
using namespace std;
//using std::cout;
//using std::string;
//using std::set;
//using std::map;
//using std::bitset;
//using std::stringstream;
//using std::vector;
//using std::pair;

bool debugging_enabled = false;

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
	if (argc < 2) {
		cout << "Usage: ./solosat.exe relPath filepath\n";
		return -1;
	}
	string relPath("");
	string filePath(argv[1]);
	if(argc == 3) {
		relPath = argv[1];
		filePath = argv[2];
	}
  	path graphFile(relPath+filePath);
	std::ofstream debugo("debug.debug", std::ofstream::out | std::ofstream::trunc);

  	
  	try {
		if (exists(graphFile)) {
			if (!is_regular_file(graphFile)) {
				debugo << "Error: Not a valid file\n";
				return -1;
			}
    	} else {
    		debugo << graphFile << " does not exist\n";
    		return -1;
    	}
  	} catch (const filesystem_error& ex) {
		debugo << ex.what() << '\n';
		return -1;
  	}

	steady_clock::time_point begin =steady_clock::now();
	steady_clock::time_point end=steady_clock::now();
	steady_clock::time_point stop =steady_clock::now();

	path fullPath(initial_path());
	DEBUG("fullPath: " << fullPath.string());
	fullPath = system_complete( path( argv[0] ) );
	DEBUG("fullPath: " << fullPath.string());

	path solverRoot = fullPath.parent_path();

	//string root("../../");
	//string solverRoot(root+"solvers/solosat/");
	path solverTmp(solverRoot / "tmp");

	//string baseFilename = strArg.substr(strArg.find_last_of("/\\") + 1);
	path baseFile = solverTmp / graphFile.stem();
	path clausesFile(baseFile.string()+".clauses");
	path resultsFile(baseFile.string()+".result");

	boost::filesystem::ifstream infile(graphFile);
	boost::filesystem::ofstream debug(baseFile.string()+".debug", std::ofstream::out | std::ofstream::trunc);

	string satCall(solverRoot.string()+"/bin/riss -config=Riss427 -verb=0 -quiet "+clausesFile.string()+" "+resultsFile.string()+" > /dev/null 2>&1");
	DEBUG("graphFile: " << graphFile.string());
	DEBUG("solverRoot: " << solverRoot.string());
	DEBUG("solverTmp: " << solverTmp.string());
	DEBUG("baseFile: " << baseFile.string());
	DEBUG("clausesFile: " << clausesFile.string());
	DEBUG("resultsFile: " << resultsFile.string());

	//system(("chmod 666 "+solverTmp+baseFilename+".debug").c_str());
	string line;
	set<pair<unsigned, unsigned>> edges;
	map<pair<unsigned, unsigned>, bool> mappedEdges;

	unsigned nodes;
	//unsigned nr_of_edges;
	while (getline(infile, line))
	{
		vector<string> tokens = split(line, ' ');

		if (tokens[0][0] == 'e' && tokens.size() == 3) {
			pair<unsigned, unsigned> p = make_pair(stoul(tokens[1]), stoul(tokens[2]));
			edges.insert(p);
			mappedEdges.insert(make_pair(p, true));
		} else if(tokens[0][0] == 'p' && tokens.size() == 4) {
			nodes = stoul(tokens[2]);
			//nr_of_edges = stoul(tokens[3]);
		}

	}
	end=steady_clock::now();
	debug << "Time after parsing (ms): " <<duration_cast<milliseconds>(end - stop).count() << endl;
	debug << "Total time (ms): " <<duration_cast<milliseconds>(end - begin).count() << endl;
	stop =steady_clock::now();


	unsigned clauses = 0;
	unsigned oldclauses = 0;
	unsigned circlelength = nodes+1;

	map<pair<unsigned, unsigned>, int> desc_to_comp;
	map<int, pair<unsigned, unsigned>> comp_to_desc;

	int var=1;
	for(unsigned i=1; i<=circlelength; i++) {
		for(unsigned v=1; v<=nodes; v++) {
			desc_to_comp.insert(make_pair(make_pair(i,v), var));
			comp_to_desc.insert(make_pair(var, make_pair(i,v)));
			var++;
		}
	}
	end=steady_clock::now();
	debug << "Time after mapping (ms): " <<duration_cast<milliseconds>(end - stop).count() << endl;
	debug << "Total time (ms): " <<duration_cast<milliseconds>(end - begin).count() << endl;
	stop =steady_clock::now();


	stringstream ss;
	vector<string> lines;

	DEBUG("cond0 ");
	ss << "c cond0" << endl;
	ss << desc_to_comp.at(make_pair(1, 1)) << ' ' << 0 << endl;
	clauses++;

	//Necessary? Test it!

	for(unsigned i=2; i<=circlelength-1; i++) {
		ss << '-' << desc_to_comp.at(make_pair(i, 1)) << ' ' << 0 << endl;
		clauses++;
	}
	for(unsigned v=2; v<=nodes; v++) {
		ss << '-' << desc_to_comp.at(make_pair(1, v)) << ' ' << 0 << endl;
		clauses++;
		ss << '-' << desc_to_comp.at(make_pair(circlelength, v)) << ' ' << 0 << endl;
		clauses++;
	}

	//End test


	ss << desc_to_comp.at(make_pair(circlelength, 1)) << ' ' << 0 << endl;

	clauses++;

	end=steady_clock::now();
	debug << "Time after cond0 (ms): " <<duration_cast<milliseconds>(end - stop).count() << endl;
	debug << "Total time (ms): " <<duration_cast<milliseconds>(end - begin).count() << endl;
	stop =steady_clock::now();

	DEBUG("c Clause diff " << clauses-oldclauses);
	ss << "c Clause diff " << clauses-oldclauses << endl;
	oldclauses = clauses;
	DEBUG("c Clauses " << clauses);
	ss << "c Clauses " << clauses << endl;

	DEBUG("c cond1");

	ss << "c cond1" << endl;
	for(unsigned v = 2; v<=nodes; v++) {
		for(unsigned i = 2; i<=circlelength-1; i++) {
			ss << desc_to_comp.at(make_pair(i, v)) << ' ';
		}
		ss << 0 << endl;
		clauses++;
	}
	end=steady_clock::now();
	debug << "Time after cond1 (ms): " <<duration_cast<milliseconds>(end - stop).count() << endl;
	debug << "Total time (ms): " <<duration_cast<milliseconds>(end - begin).count() << endl;
	stop =steady_clock::now();

	DEBUG("c Clause diff " << clauses-oldclauses);
	ss << "c Clause diff " << clauses-oldclauses << endl;
	oldclauses = clauses;
	DEBUG("c Clauses " << clauses);
	ss << "c Clauses " << clauses << endl;

	DEBUG("c cond2");
	ss << "c cond2" << endl;

	for(unsigned v = 2; v<=nodes; v++) {
		for(unsigned i = 2; i<=circlelength-2; i++) {
			for(unsigned j = i+1; j<=circlelength-1; j++) {
				ss << '-' << desc_to_comp.at(make_pair(i, v)) << ' ' << '-' << desc_to_comp.at(make_pair(j, v)) << ' ' << 0 << endl;
				clauses++;
			}
		}
	}
	end=steady_clock::now();
	debug << "Time after cond2 (ms): " <<duration_cast<milliseconds>(end - stop).count() << endl;
	debug << "Total time (ms): " <<duration_cast<milliseconds>(end - begin).count() << endl;
	stop =steady_clock::now();

	DEBUG("c Clause diff " << clauses-oldclauses);
	ss << "c Clause diff " << clauses-oldclauses << endl;
	oldclauses = clauses;
	DEBUG("c Clauses " << clauses);
	ss << "c Clauses " << clauses << endl;

	DEBUG("c cond3 ");
	ss << "c cond3" << endl;
	for(unsigned i = 2; i<=circlelength-1; i++) {
		for(unsigned v = 2; v<=nodes; v++) {
			ss << desc_to_comp.at(make_pair(i, v)) << ' ';
		}
		ss << 0 << endl;
		clauses++;
	}
	end=steady_clock::now();
	debug << "Time after cond3 (ms): " <<duration_cast<milliseconds>(end - stop).count() << endl;
	debug << "Total time (ms): " <<duration_cast<milliseconds>(end - begin).count() << endl;
	stop =steady_clock::now();


	DEBUG("c Clause diff " << clauses-oldclauses);
	ss << "c Clause diff " << clauses-oldclauses << endl;
	oldclauses = clauses;
	DEBUG("c Clauses");
	ss << "c Clauses " << clauses << endl;

	DEBUG("c cond4 ");
	ss << "c cond4" << endl;
	for(unsigned i = 2; i<=circlelength-1; i++) {
		for(unsigned v = 2; v<=nodes-1; v++) {
			for(unsigned w = v+1; w<=nodes; w++) {
				ss << '-' << desc_to_comp.at(make_pair(i, v)) << ' ' << '-' << desc_to_comp.at(make_pair(i, w)) << ' ' << 0 << endl;
				clauses++;
			}
		}
	}
	end=steady_clock::now();
	debug << "Time after cond4 (ms): " <<duration_cast<milliseconds>(end - stop).count() << endl;
	debug << "Total time (ms): " <<duration_cast<milliseconds>(end - begin).count() << endl;
	stop =steady_clock::now();


	DEBUG("c Clause diff " << clauses-oldclauses);
	ss << "c Clause diff " << clauses-oldclauses << endl;
	oldclauses = clauses;
	DEBUG("c Clauses");
	ss << "c Clauses " << clauses << endl;


	DEBUG("c cond5 ");
	ss << "c cond5" << endl;
	for(unsigned v = 1; v<=nodes; v++) {
		for(unsigned w = 1; w<=nodes; w++) {
			if(!mappedEdges.count(make_pair(v,w))) {
				for(unsigned i = 1; i<=circlelength-1; i++) {
					ss << '-' << desc_to_comp.at(make_pair(i, v)) << ' ' << '-' << desc_to_comp.at(make_pair(i+1, w)) << ' ' << 0 << endl;
					clauses++;
				}
			}
		}
	}
	end=steady_clock::now();
	debug << "Time after cond5 call (ms): " <<duration_cast<milliseconds>(end - stop).count() << endl;
	debug << "Total time (ms): " <<duration_cast<milliseconds>(end - begin).count() << endl;
	stop =steady_clock::now();


	DEBUG("c Clause diff " << clauses-oldclauses);
	ss << "c Clause diff " << clauses-oldclauses << endl;
	oldclauses = clauses;
	DEBUG("c Clauses " << clauses);
	ss << "c Clauses " << clauses << endl;

	/*
	DEBUG("c cond6");

	ss << "c cond6" << endl;
	for(unsigned i=0; i<pow(2, nodes); i++) {
		for(unsigned v=1; v<=nodes; v++) {
			unsigned pos = (((i >> (v-1)) & 1) ? circlelength : 1);
			ss << desc_to_comp.at(make_pair(pos, v)) << ' ';
		}
		ss << 0 << endl;
		clauses++;
	}
	DEBUG("c Clause diff " << clauses-oldclauses);
	ss << "c Clause diff " << clauses-oldclauses << endl;
	oldclauses = clauses;
	DEBUG("c Clauses " << clauses);
	ss << "c Clauses " << clauses << endl;
*/
	//string filename = root+"tmp/"+argv[1]+".clauses";

	boost::filesystem::ofstream outfile(clausesFile, std::ofstream::out | std::ofstream::trunc);
	//system(("chmod 666 "+filename).c_str());

	//outfile << "p cnf " << nodes*circlelength << ' ' << clauses << endl;
	outfile << ss.rdbuf();

	outfile.close();

	end=steady_clock::now();
	debug << "Time before solver call (ms): " <<duration_cast<milliseconds>(end - stop).count() << endl;
	debug << "Total time (ms): " <<duration_cast<milliseconds>(end - begin).count() << endl;
	stop =steady_clock::now();

	system((satCall).c_str());

	end=steady_clock::now();
	debug << "Time after solver call (ms): " <<duration_cast<milliseconds>(end - stop).count() << endl;
	debug << "Total time (ms): " <<duration_cast<milliseconds>(end - begin).count() << endl;
	stop =steady_clock::now();

	DEBUG("finished solving ");

	boost::filesystem::ifstream resultsstream(resultsFile);

	getline(resultsstream, line);
	stringstream firstLine(line);

	string result;
	getline(firstLine, result);
	DEBUG("This is the result: " << result);
	if(result == "s UNSATISFIABLE") {
		cout << "s UNSATISFIABLE";
		return 20;
	} else if(result == "s SATISFIABLE") {

		getline(resultsstream, line);
		stringstream secondLine(line);

	/*
		for(std::map<int,pair<unsigned, unsigned>>::iterator 	iter=comp_to_desc.begin(); iter != comp_to_desc.end(); ++iter)
		{
		cout << "Key: " << iter->first << " Value path: "<< iter->second.first << " Value node: " << iter->second.second << endl;;
		}
	*/
		cout << "s SATISFIABLE" << endl;
		cout << "v ";
		unsigned addedNodes = 0;
		vector<int> path;
		//Skip initial 'v'
		getline(secondLine, result, ' ');
		while(getline(secondLine, result, ' ')) {
			if(result == "0") {
				break;
			}
			if(result.c_str()[0] != '-') {
	//			string node = result.substr(result.find(to_string(addedNodes+1))+1);
				int node = comp_to_desc.at(atoi(result.c_str())).second;
				cout << node << ' ';
				addedNodes++;
				if(addedNodes == circlelength-1) {
					break;
				}
			}
		}

		return 10;
	} else {
		return -1;
	}
//	for (set<pair<unsigned,unsigned>>::iterator it=edges.begin(); it!=edges.end(); ++it)
//		 DEBUG("From: " << it->first << " To: " << it->second);

}

