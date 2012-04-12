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
#include <InHistoryTree.hpp>
#include <OutHistoryTree.hpp>
#include <HistoryTree.hpp>
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

class TreeTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TreeTest );
	
	CPPUNIT_TEST( testWrite );
	CPPUNIT_TEST( testProperties );
	CPPUNIT_TEST( testRead );
	CPPUNIT_TEST( testSparseRead );
	CPPUNIT_TEST( testInexistentFile );
	CPPUNIT_TEST( testBadExistingFile );
	
	CPPUNIT_TEST_SUITE_END();
	
private:
		
	OutHistoryTree* oht;
	InHistoryTree* iht;
	HistoryTree* ht;
	
	AbstractInterval::SharedPtr interval1;
	AbstractInterval::SharedPtr interval2;
	AbstractInterval::SharedPtr interval3;
	AbstractInterval::SharedPtr interval4;
	AbstractInterval::SharedPtr interval5;
	
	std::multiset<AbstractInterval::SharedPtr> intervals;
	
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
	
	/**
	 * Tests whether a given queryResult corresponds to the expected results
	 * 
	 */ 
	bool verify(std::multimap<attribute_t, AbstractInterval::SharedPtr> queryResult, timestamp_t timestamp)
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
		for(std::multimap<attribute_t, AbstractInterval::SharedPtr>::const_iterator it = queryResult.begin(); it != queryResult.end(); it++)
		{
			if (it->second != 0)
			{
				bool found = false;
				for(std::vector<std::pair<AbstractInterval::SharedPtr, bool> >::iterator it2 = expected.begin(); it2 != expected.end(); it2++)
				{
					// We have found the query result inside the expected result, mark it as such
					if(compare(it2->first, it->second))
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
	
public:
	void setUp()
	{			
		oht = new OutHistoryTree(HistoryTreeConfig());
		iht = new InHistoryTree(HistoryTreeConfig());
		ht = new HistoryTree(HistoryTreeConfig());
		
		interval1.reset(new IntInterval(0,10,5,20));
		interval2.reset(new StringInterval(5,11,4,"value"));
		interval3.reset(new UIntInterval(2,9,10,900));
		interval4.reset(new FloatInterval(9,13,6,2.0));
		interval5.reset(new NullInterval(11,12,2));
		
		intervals.insert(interval1);
		intervals.insert(interval2);
		intervals.insert(interval3);
		intervals.insert(interval4);
		intervals.insert(interval5);
	}

	void tearDown() 
	{
		delete oht;
		delete iht;
		delete ht;
	}
	
	void testWrite()
	{
		oht->setConfig(HistoryTreeConfig("./o.ht", 128, 3, 0));
		oht->open();
		
		
		oht->addInterval(interval1);
		oht->addInterval(interval2);
		oht->addInterval(interval3);
		oht->addInterval(interval4);
		oht->addInterval(interval5);
		
		oht->close();
	}
	
	void testProperties()
	{
		iht->setConfig(HistoryTreeConfig("./o.ht", 128, 3, 0));
		iht->open();
		
		CPPUNIT_ASSERT(13 == iht->getEnd());
		CPPUNIT_ASSERT(3 == iht->_node_count);
		
		iht->close();
	}
	
	void testRead()
	{
		iht->setConfig(HistoryTreeConfig("./o.ht", 128, 3, 0));
		try{
			iht->open();
		}catch(...)
		{
			CPPUNIT_FAIL("Could not open tree for reading");
		}
		
		std::vector< AbstractInterval::SharedPtr > queryResult;		
		
		for (timestamp_t i = 0; i < 14; i++)
		{
			try{
				queryResult = iht->query(i);
				//outputQuery(queryResult, i);
			}catch(TimeRangeEx& ex){
				CPPUNIT_FAIL("Reading inside bounds resulted in out-of-bound error");
			}catch(...){
				CPPUNIT_FAIL("Critical error during read");
			}
			CPPUNIT_ASSERT(verify(queryResult, i));
		}
		CPPUNIT_ASSERT_THROW(iht->query(15), TimeRangeEx);
		CPPUNIT_ASSERT_THROW(iht->query(-1), TimeRangeEx);
	}
	
	void testSparseRead()
	{
		iht->setConfig(HistoryTreeConfig("./o.ht", 128, 3, 0));
		try{
			iht->open();
		}catch(...)
		{
			CPPUNIT_FAIL("Could not open tree for reading");
		}
		
		std::multimap<attribute_t, AbstractInterval::SharedPtr > queryResult;		
		
		for (timestamp_t i = 0; i < 14; i++)
		{
			try{
				queryResult = iht->sparseQuery(i);
				//outputQuery(queryResult, i);
			}catch(TimeRangeEx& ex){
				CPPUNIT_FAIL("Reading inside bounds resulted in out-of-bound error");
			}catch(...){
				CPPUNIT_FAIL("Critical error during read");
			}
			CPPUNIT_ASSERT(verify(queryResult, i));
		}
		CPPUNIT_ASSERT_THROW(iht->query(15), TimeRangeEx);
		CPPUNIT_ASSERT_THROW(iht->query(-1), TimeRangeEx);
	}	
	
	void testInexistentFile()
	{
		// Clear any existing file
		remove("./o.ht");
		remove("./i.ht");
		remove("./io.ht");
		
		
		HistoryTreeConfig oConfig("./o.ht", 128, 3, 0);
		oht->setConfig(oConfig);
		
		try{
			oht->open();
		}catch(IOEx& ex){
			CPPUNIT_FAIL("Could not open file for writing");
		}
		oht->close(1);		
		
		HistoryTreeConfig iConfig("./i.ht", 128, 3, 0);
		iht->setConfig(iConfig);
		CPPUNIT_ASSERT_THROW(iht->open(), IOEx);		
		
		HistoryTreeConfig ioConfig("./io.ht", 128, 3, 0);
		ht->setConfig(ioConfig);
		try{
			ht->open();
		}catch(InvalidFormatEx& ex){
			CPPUNIT_FAIL("Invalid header");
		}catch(IOEx& ex){
			CPPUNIT_FAIL("Could not open file for writing");
		}
		ht->close();
	}
	
	void testBadExistingFile()
	{
		fstream f("./o.ht");
		f.seekp(0);
		f.write("problem?", 8);
		f.close();
		
		HistoryTreeConfig iConfig("./o.ht", 128, 3, 0);
		iht->setConfig(iConfig);
		CPPUNIT_ASSERT_THROW(iht->open(), InvalidFormatEx);
	}
	
};

CPPUNIT_TEST_SUITE_REGISTRATION( TreeTest );
