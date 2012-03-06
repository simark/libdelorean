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
	
	cout << "BEFORE:\n";
	ht->test();
	
	// leaf (seq 0)
	ht->addInterval(IntervalSharedPtr(new IntInterval(2, 7, 0, 32)));
	ht->addInterval(IntervalSharedPtr(new IntInterval(5, 11, 1, 33)));
	ht->addInterval(IntervalSharedPtr(new IntInterval(3, 25, 2, 34)));
	
	// leaf (seq 2)
	ht->addInterval(IntervalSharedPtr(new IntInterval(28, 35, 3, 35)));
	ht->addInterval(IntervalSharedPtr(new IntInterval(32, 45, 4, 36)));
	
	// core (seq 1)
	ht->addInterval(IntervalSharedPtr(new IntInterval(16, 50, 5, 37)));
	
	// leaf (seq 2)
	ht->addInterval(IntervalSharedPtr(new IntInterval(26, 52, 6, 38)));
	
	// leaf (seq 3)
	ht->addInterval(IntervalSharedPtr(new IntInterval(65, 70, 7, 39)));
	ht->addInterval(IntervalSharedPtr(new IntInterval(56, 74, 8, 40)));
	
	// core (seq 4)
	ht->addInterval(IntervalSharedPtr(new IntInterval(35, 76, 9, 40)));
	
	// leaf (seq 6)
	ht->addInterval(IntervalSharedPtr(new IntInterval(80, 85, 10, 41)));
	ht->addInterval(IntervalSharedPtr(new IntInterval(76, 88, 11, 42)));
	ht->addInterval(IntervalSharedPtr(new IntInterval(90, 10, 12, 43)));
	
	// leaf (seq 7)
	ht->addInterval(IntervalSharedPtr(new IntInterval(105, 110, 13, 44)));
	
	// core (seq 5)
	ht->addInterval(IntervalSharedPtr(new IntInterval(77, 120, 14, 45)));
	
	// leaf (seq 7)
	ht->addInterval(IntervalSharedPtr(new IntInterval(102, 137, 15, 46)));
	
	cout << "AFTER:\n";
	ht->test();
	
	// close tree
	ht->close(100);
	delete ht;
}

int main(void) {
	//testInHT();
	//testQuery();
	testInOutHT();
	
	return 0;
}
