#include <tr1/memory>
#include <iostream>
#include <fstream>
#include <exception>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include <intervals/IntIntervalFactory.hpp>
#include <intervals/IntInterval.hpp>
#include <IntervalCreator.hpp>
#include <intervals/StringInterval.hpp>
#include <AbstractHistoryTree.hpp>
#include <InHistoryTree.hpp>
#include <HistoryTree.hpp>
#include <HistoryTreeConfig.hpp>
#include <ThreadedOutHistoryTree.hpp>

using namespace std;
using namespace std::tr1;

const unsigned int NB_INSERT=100000000;

void static stressTest() {
	HistoryTreeConfig config("./out.ht", 0);
	OutHistoryTree* oht = new ThreadedOutHistoryTree(config);
	oht->open();
	for(unsigned int i = 10; i < NB_INSERT; ++i) {
		unsigned int start = i - rand() % 10;
		AbstractInterval::SharedPtr iint(new IntInterval(start, i, i, i));
		*oht << iint;
		AbstractInterval::SharedPtr sint(new StringInterval(start, i, i+NB_INSERT, "woah"));
		*oht << sint;
	}
	oht->close();
}

int main(void) {
	stressTest();
	
	return 0;
}
