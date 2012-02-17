#include <tr1/memory>
#include <iostream>
#include <exception>
#include <cstring>
#include <cstdio>

#include <IntIntervalFactory.hpp>
#include <IntInterval.hpp>
#include <IntervalCreator.hpp>
#include <StringInterval.hpp>
#include <HistoryTreeNode.hpp>
#include <HistoryTree.hpp>
#include <HistoryTreeConfig.hpp>

using namespace std;
using namespace std::tr1;

static void printTestHeader(string what) {
	cout << endl << "### " << what << endl;
}

static void testInterval(void) {
	printTestHeader("testInterval");
	
	// create factory
	IIntervalFactory* factory = new IntIntervalFactory();
	
	// create interval
	IntervalSharedPtr interval = factory->create();
	delete factory;
	
	// set attributes
	interval->setStart(23)->setEnd(119)->setAttribute(11);
	int32_t val = -4123;
	interval->unserialize(NULL, &val);	
	
	// printing (debugging)
	cout << "printing:\n" << endl;
	cout << *interval << endl;
	
	// intersects
	cout << "intersects:\n" << endl;
	cout << "22 : " << interval->intersects(22) << endl <<
		"23 : " << interval->intersects(23) << endl <<
		"24 : " << interval->intersects(24) << endl <<
		"118 : " << interval->intersects(118) << endl <<
		"119 : " << interval->intersects(119) << endl <<
		"120 : " << interval->intersects(120) << endl;
}

static void testStringInterval(void) {
	printTestHeader("testStringInterval");
	
	char somebuf [] = "    here is a string";
	uint32_t len = strlen(somebuf) - 4;
	memcpy(somebuf, &len, 4);
	IntervalSharedPtr interval(new StringInterval());
	interval->unserialize(somebuf, NULL);
	interval->setInterval(13, 4657);
	interval->setAttribute(1422);
	cout << *interval << endl;
}

static void testIntervalCreator(void) {
	printTestHeader("testIntervalCreator");
	
	IntervalCreator creator;
	IntervalSharedPtr interval;
	
	try {
		creator.createIntervalFromType(1);
		creator.createIntervalFromType(5);
	} catch (const exception& ex) {
		cout << ex.what() << endl;
	}
}

static void hexDump(unsigned char* h, unsigned int n) {
	for (unsigned int d = 0; d < n; ++d) {
		if (d % 16 == 0) {
			printf("\n");
		}
		printf("%02x ", (unsigned int) h[d]);
	}
	printf("\n");
	fflush(stdout);
}

static void testNodeDump(void) {
	printTestHeader("testNodeDump");
		
	IntInterval int_interval(2, 18, 7, 65);
	StringInterval str_interval(5, 25, 6, "allo vous!");
	
	HistoryTreeConfig config;
	config._stateFile = "/tmp/lol";
	config._blockSize = 4096;
	config._maxChildren = 10;
	config._treeStart = 0;
	
	HistoryTree tree(config);
	
	HistoryTreeNode node(tree, 34, 14, 1);
	node.addInterval(str_interval);
	node.addInterval(int_interval);
	node.closeThisNode(100);
	
	cout << "node free space: " << node.getFreeSpace() << endl;
	
	uint8_t buf [4096];
	node.serialize(buf);
	hexDump(buf, 4096);
}

int main(void) {
	testInterval();
	testIntervalCreator();
	testStringInterval();
	testNodeDump();
	
	return 0;
}
