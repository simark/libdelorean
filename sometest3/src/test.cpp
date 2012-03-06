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
#include <InHistoryTree.hpp>
#include <HistoryTreeConfig.hpp>

using namespace std;
using namespace std::tr1;

static void printTestHeader(string what) {
	cout << endl << "### " << what << endl;
}

static void testQuery(void) {
	printTestHeader("testQuery");
	
	// some config
	HistoryTreeConfig config("./out.ht");
	
	// input history tree
	InHistoryTree* iht = new InHistoryTree(config);
	iht->open();
	
	vector<IntervalSharedPtr> intervals;
	intervals = iht->query(20);
	for(vector<IntervalSharedPtr>::iterator it = intervals.begin(); it != intervals.end(); it++)
	{
		if(*it != 0)
			cout << **it << endl;
	}
	
	// close tree
	iht->close();
	delete iht;
}

static void testInHT(void) {
	printTestHeader("testInHT");
	
	// some config
	HistoryTreeConfig config("./out.ht");
	
	// input history tree
	InHistoryTree* iht = new InHistoryTree(config);
	iht->open();
	
	iht->test();
	
	// close tree
	iht->close();
	delete iht;
}

int main(void) {
	testInHT();
	testQuery();
	
	return 0;
}
