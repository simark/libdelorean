#include <tr1/memory>
#include <iostream>
#include <fstream>
#include <exception>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include <IntIntervalFactory.hpp>
#include <IntInterval.hpp>
#include <IntervalCreator.hpp>
#include <StringInterval.hpp>
#include <AbstractHistoryTree.hpp>
#include <OutHistoryTree.hpp>
#include <HistoryTreeConfig.hpp>

using namespace std;
using namespace std::tr1;

static void printTestHeader(string what) {
	cout << endl << "### " << what << endl;
}

static void addIntInterval(OutHistoryTree* oht, timestamp_t start, timestamp_t end) {
	static int32_t val = 32;
	static attribute_t attr = 0;
	shared_ptr<IntInterval> inti(new IntInterval(start, end, attr, val));
	*oht << inti;
	++val;
	++attr;
}

static void testOutHT(void) {
	printTestHeader("testOutHT");
	
	// some config
	// with those parameters, only 3 IntInterval fit into a leaf node and
	// 1 into a core node
	HistoryTreeConfig config("./out.ht", 128, 4, 0);
	
	// output history tree
	OutHistoryTree* oht = new OutHistoryTree(config);
	config = oht->getConfig();
	oht->open();
	
	// create intervals and add them
	
	// leaf (seq 0)
	addIntInterval(oht, 2, 7);
	addIntInterval(oht, 5, 11);
	addIntInterval(oht, 3, 25);
	
	// leaf (seq 2)
	addIntInterval(oht, 28, 35);
	addIntInterval(oht, 32, 45);
	
	// core (seq 1)
	addIntInterval(oht, 16, 50);
	
	// leaf (seq 2)
	addIntInterval(oht, 26, 52);
	
	// leaf (seq 3)
	addIntInterval(oht, 65, 70);
	addIntInterval(oht, 56, 74);
	
	// core (seq 4)
	addIntInterval(oht, 35, 76);
	
	// leaf (seq 6)
	addIntInterval(oht, 80, 85);
	addIntInterval(oht, 76, 88);
	addIntInterval(oht, 90, 100);
	
	// leaf (seq 7)
	addIntInterval(oht, 105, 110);
	
	// core (seq 5)
	addIntInterval(oht, 77, 120);
	
	// leaf (seq 7)
	addIntInterval(oht, 102, 137);
	
	// close tree
	oht->close();
	delete oht;
}

int main(void) {
	testOutHT();
	
	return 0;
}
