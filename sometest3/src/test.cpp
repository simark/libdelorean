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
#include <HistoryTree.hpp>
#include <HistoryTreeConfig.hpp>

using namespace std;
using namespace std::tr1;

static void printTestHeader(string what) {
	cout << endl << "### " << what << endl;
}

static void queryAndPrint(InHistoryTree* iht, int t)
{
	cout << "Request at t=" << t << endl;
	vector<IntervalSharedPtr> intervals;
	try{
		intervals = iht->query(t);
	}catch(TimeRangeEx& ex){
		cout << ex.what() << endl;
	}
	for(vector<IntervalSharedPtr>::iterator it = intervals.begin(); it != intervals.end(); it++)
	{
		if(*it != 0)
			cout << **it << endl;
	}
}

static void testQuery(void) {
	printTestHeader("testQuery");
	
	// some config
	HistoryTreeConfig config("./out.ht");
	
	// input history tree
	InHistoryTree* iht = new InHistoryTree(config);
	iht->open();
	
	for (int i = 0; i < 160; i+=10)
	{
		queryAndPrint(iht, i);
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

static void testInOutHT(void) {
	printTestHeader("testInOutHT");
	// some config
	HistoryTreeConfig config("./inout.ht", 128, 4, 0);
	
	// input history tree
	HistoryTree* ht = new HistoryTree(config);
	ht->open();
	
	ht->addInterval(IntervalSharedPtr(new IntInterval(10, 15, 0, 12)));
	
	ht->test();
	
	// close tree
	ht->close(100);
	delete ht;
}

int main(void) {
	testInHT();
	testQuery();
	testInOutHT();
	
	return 0;
}
