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
#include <ThreadedInHistoryTree.hpp>
#include <MemoryInHistoryTree.hpp>

using namespace std;
using namespace std::tr1;

const unsigned int NB_QUERY=10000;

void static stressTest() {
	std::vector<AbstractInterval::SharedPtr> results;
	std::multimap<attribute_t, AbstractInterval::SharedPtr> sparseResults;
	HistoryTreeConfig config("../../perftest/src/out.ht", 0);
	InHistoryTree* iht = new InHistoryTree(config);
	iht->open();
	unsigned int nbFail = 0;
	for(unsigned int i = 10; i < NB_QUERY; ++i) {
		unsigned int start = rand() % (NB_QUERY-10) + 10;
		AbstractInterval::SharedPtr ptr = iht->query(start, start);
		if (ptr == NULL)
			nbFail++;
		else
			std::cout << *ptr << std::endl;
	}
	iht->close();
	delete iht;
	std::cout << nbFail << std::endl;
}

int main(void) {
	stressTest();
	
	return 0;
}
