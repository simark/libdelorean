#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include <istream>
#include <ostream>
#include <iostream>
#include <string>
#include <cstring>
#include <cmath>

#define protected public
#define private public
#include <MemoryInHistoryTree.hpp>
#include <MemoryOutHistoryTree.hpp>
#include <MemoryHistoryTree.hpp>
#undef protected
#undef private

#include <intervals/IntInterval.hpp>
#include <intervals/StringInterval.hpp>
#include <intervals/UIntInterval.hpp>
#include <intervals/FloatInterval.hpp>
#include <intervals/NullInterval.hpp>

#include <ex/IOEx.hpp>
#include <ex/InvalidFormatEx.hpp>

#include <fstream>
#include <cstdio>
#include <set>

using namespace std;

class MemoryTreeTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( MemoryTreeTest );
	
	CPPUNIT_TEST( testConcurrentWrite );
	CPPUNIT_TEST( testConcurrentRead );
	CPPUNIT_TEST( testConcurrentReadWrite );
	
	CPPUNIT_TEST_SUITE_END();
	
private:

	static const unsigned int INSERTS_PER_THREAD = 1000;
	static const unsigned int NB_THREADS = 10;
	
		
	OutHistoryTree* oht;
	InHistoryTree* iht;
	HistoryTree* ht;
	
	AbstractInterval::SharedPtr interval1;
	AbstractInterval::SharedPtr interval2;
	AbstractInterval::SharedPtr interval3;
	AbstractInterval::SharedPtr interval4;
	AbstractInterval::SharedPtr interval5;
	
	std::multiset<AbstractInterval::SharedPtr> intervals;
	
	std::vector<AbstractInterval::SharedPtr> _insertedVector[NB_THREADS];
	
	/**
	 * Compare 2 intervals' HEADERS only. The values are not compared.
	 * 
	 */ 
	bool compare(AbstractInterval::SharedPtr i1, AbstractInterval::SharedPtr i2)
	{
		if(i1->getStart() == i2->getStart() && i1->getEnd() == i2->getEnd() && i1->getAttribute() == i2->getAttribute() && i1->getType() == i2->getType())
		{
			return true;
		}
		return false;
	}
	
	/**
	 * Tests whether a given queryResult corresponds to the expected results
	 * 
	 */ 
	bool verify(std::vector<AbstractInterval::SharedPtr> queryResult, timestamp_t timestamp)
	{
		// Construct the expected result vector
		std::vector<std::pair<AbstractInterval::SharedPtr, bool> > expected;
		for(std::multiset<AbstractInterval::SharedPtr>::const_iterator it = intervals.begin(); it != intervals.end(); it++)
		{
			if((*it)->getStart() <= timestamp && (*it)->getEnd() >= timestamp)
			{
				expected.push_back(std::make_pair(*it, false));
			}
		}
		
		// Find each query result inside the expected result vector
		for(std::vector<AbstractInterval::SharedPtr>::const_iterator it = queryResult.begin(); it != queryResult.end(); it++)
		{
			if (*it != 0)
			{
				bool found = false;
				for(std::vector<std::pair<AbstractInterval::SharedPtr, bool> >::iterator it2 = expected.begin(); it2 != expected.end(); it2++)
				{
					// We have found the query result inside the expected result, mark it as such
					if(compare(it2->first, *it))
					{
						it2->second = true;
						found = true;
					}
				}
				if (!found) return false;
			}
		}
		
		// Every expected result should have been found
		for(std::vector<std::pair<AbstractInterval::SharedPtr, bool> >::const_iterator it = expected.begin(); it != expected.end(); it++)
		{
			if (it->second == false)
				return false;
		}
		return true;
		
	}
	
	void outputQuery(std::vector<AbstractInterval::SharedPtr> queryResult, timestamp_t timestamp){
		cout << "Request result at t=" << timestamp << endl;
		for(std::vector<AbstractInterval::SharedPtr>::const_iterator it = queryResult.begin(); it != queryResult.end(); it++){
			if(*it != 0){
				cout << **it << endl;
			}
		}
	}
	
	void writeIntervals(OutHistoryTree* outTree, attribute_t start){
		for(unsigned int i = 0; i < INSERTS_PER_THREAD; i++){
			AbstractInterval::SharedPtr interval(new NullInterval(0,i,start+i));
			outTree->addInterval(interval);
		}
	}
	
	void readIntervals(InHistoryTree* inTree, timestamp_t timestamp, vector<AbstractInterval::SharedPtr> expected, bool* success){
		
		vector<AbstractInterval::SharedPtr> queryResult = inTree->query(timestamp);
		vector<bool> individualSuccess(expected.size(), false);
		
		for(vector<AbstractInterval::SharedPtr>::const_iterator it = queryResult.begin(); it!= queryResult.end(); it++){
			if (*it != 0){
				bool found = false;
				for(vector<AbstractInterval::SharedPtr>::const_iterator it2 = expected.begin(); it2!= expected.end(); it2++){
					if( (*it)->getStart() == (*it2)->getStart() && (*it)->getEnd() == (*it2)->getEnd() && (*it)->getAttribute() == (*it2)->getAttribute()){
						individualSuccess[it2-expected.begin()] = true;
						found = true;
					}
				}
				if (!found){
					*success = false;
					return;
				}
			}
		}
		
		for(vector<bool>::const_iterator it = individualSuccess.begin(); it!= individualSuccess.end(); it++){
			if(! (*it)){
				*success = false;
				return;
			}
		}
		*success = true;
		return;
	}	
	
	void superWrite(OutHistoryTree* outTree){
		for(unsigned int i = 0; i < 10000; i++){
			(*outTree) << (AbstractInterval::SharedPtr(new NullInterval(i, i+10, (i+i+10)/2)));
		}
	}
	
	void superRead(InHistoryTree* inTree, bool* result){
		bool success = true;
		for(unsigned int i = 0; i < 10000; i++){
			try{
				vector<AbstractInterval::SharedPtr> queryResult = inTree->query(rand()%10000);
				for(vector<AbstractInterval::SharedPtr>::const_iterator it = queryResult.begin(); it != queryResult.end(); it++){
					if (*it != 0){
						if((*it)->getStart() != (*it)->getEnd() - 10 && (*it)->getAttribute() != ((*it)->getStart()+(*it)->getEnd())/2){
							success = false;
						}
					}
				}
			}catch(TimeRangeEx ex){
			}
		}
		*result = success;
	}
	
public:
	void setUp()
	{			
		oht = new MemoryOutHistoryTree(HistoryTreeConfig());
		iht = new MemoryInHistoryTree(HistoryTreeConfig());
		ht = new MemoryHistoryTree(HistoryTreeConfig());
		
		for(unsigned int att = 0; att < NB_THREADS; att++){
			
			for(unsigned int i = 0; i < INSERTS_PER_THREAD; i++){
				AbstractInterval::SharedPtr interval(new NullInterval(0,i,(INSERTS_PER_THREAD*att)+i));
				_insertedVector[att].push_back(interval);
			}
		}
	}

	void tearDown() 
	{
		delete oht;
		delete iht;
		delete ht;
	}
	
	void testConcurrentWrite()
	{
		oht->setConfig(HistoryTreeConfig("./o.ht", 512, 3, 0));
		oht->open();
		
		vector<boost::thread*> threads;
		
		//Start 10 threads
		for(unsigned int i = 0; i < NB_THREADS; i++)
			threads.push_back(new boost::thread(boost::bind(&MemoryTreeTest::writeIntervals, this, oht, INSERTS_PER_THREAD*i)));
			
		for(unsigned int i = 0; i < NB_THREADS; i++)
			threads[i]->join();
			
		for(unsigned int i = 0; i < NB_THREADS; i++){
			delete threads[i];
		}
		
		oht->close();
	}
	
	void testConcurrentRead()
	{
		iht->setConfig(HistoryTreeConfig("./o.ht", 512, 3, 0));
		try{
			iht->open();
		}catch(...)
		{
			CPPUNIT_FAIL("Could not open tree for reading");
		}
		
		std::vector<AbstractInterval::SharedPtr> expected[INSERTS_PER_THREAD];
		//For every timestamp in the tree (0-9)
		for(unsigned int i = 0; i < INSERTS_PER_THREAD; i++){
			//For every insertion thread
			for(unsigned int j = 0; j < NB_THREADS; j++){
				//Find the intervals at timestamp i
				std::vector<AbstractInterval::SharedPtr> inserted = _insertedVector[j];
				for(std::vector<AbstractInterval::SharedPtr>::const_iterator it = inserted.begin(); it!=inserted.end(); it++){
					if ( (*it)->getStart() <= i && (*it)->getEnd() >= i){
						expected[i].push_back(*it);
					}
				}
			}			
		}
		
		vector<boost::thread*> threads;
		bool success[NB_THREADS];
		
		for(unsigned int i = 0; i < NB_THREADS; i++)
		{
			success[i] = false;
			threads.push_back(new boost::thread(boost::bind(&MemoryTreeTest::readIntervals, this, iht, i, expected[i], &success[i])));
		}
		
		for(unsigned int i = 0; i < NB_THREADS; i++)
		{
			threads[i]->join();
		}
		
		for(unsigned int i = 0; i < NB_THREADS; i++)
		{
			delete threads[i];
		}
		
		for(unsigned int i = 0; i < NB_THREADS; i++)
		{
			CPPUNIT_ASSERT(true == success[i]);
		}		
		
	}
	
	void testConcurrentReadWrite()
	{
		// Clear any existing file		
		
		HistoryTreeConfig ioConfig("./io.ht", 512, 3, 0);
		ht->setConfig(ioConfig);
		
		try{
			ht->open(HistoryTree::TRUNCATE);
		}catch(IOEx& ex){
			CPPUNIT_FAIL("Could not open file for writing");
		}
		
		bool success;
		boost::thread outThread(boost::bind(&MemoryTreeTest::superWrite, this, ht));
		boost::thread inThread(boost::bind(&MemoryTreeTest::superRead, this, ht, &success));
		
		outThread.join();
		inThread.join();
		
		ht->close();
		
		CPPUNIT_ASSERT(true == success);
	}
	
};

CPPUNIT_TEST_SUITE_REGISTRATION( MemoryTreeTest );


