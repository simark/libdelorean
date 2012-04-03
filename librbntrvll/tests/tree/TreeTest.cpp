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
	CPPUNIT_TEST( testRead );
	//CPPUNIT_TEST( testInexistentFile );
	
	CPPUNIT_TEST_SUITE_END();
	
private:
		
	OutHistoryTree* oht;
	InHistoryTree* iht;
	HistoryTree* ht;
	
	Interval::SharedPtr interval1;
	Interval::SharedPtr interval2;
	Interval::SharedPtr interval3;
	Interval::SharedPtr interval4;
	Interval::SharedPtr interval5;
	
	std::multiset<Interval::SharedPtr> intervals;
	
	bool compare(Interval::SharedPtr i1, Interval::SharedPtr i2)
	{
		if(i1->getStart() == i2->getStart() && i1->getEnd() == i2->getEnd() && i1->getAttribute() == i2->getAttribute() && i1->getType() == i2->getType())
		{
			return true;
		}
		return false;
	}
	
	bool verify(std::vector<Interval::SharedPtr> queryResult, timestamp_t timestamp)
	{
		std::vector<std::pair<Interval::SharedPtr, bool> > expected;
		std::vector<bool> correct;
		for(std::multiset<Interval::SharedPtr>::const_iterator it = intervals.begin(); it != intervals.end(); it++)
		{
			if((*it)->getStart() >= timestamp && (*it)->getEnd() <= timestamp)
			{
				expected.push_back(std::make_pair(*it, false));
			}
		}
		
		for(std::vector<Interval::SharedPtr>::const_iterator it = queryResult.begin(); it != queryResult.end(); it++)
		{
			if (*it != 0)
			{
				for(std::vector<std::pair<Interval::SharedPtr, bool> >::iterator it2 = expected.begin(); it2 != expected.end(); it2++)
				{
					if(compare(it2->first, *it))
					{
						it2->second = true;
					}
				}
			}
		}
		
		for(std::vector<std::pair<Interval::SharedPtr, bool> >::const_iterator it = expected.begin(); it != expected.end(); it++)
		{
			if (it->second == false)
				return false;
		}
		return true;
		
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
		delete oht;
		oht = new OutHistoryTree();
		oht->setConfig(HistoryTreeConfig("./o.ht", 128, 3, 0));
		oht->open();
		
		oht->addInterval(interval1);
		oht->addInterval(interval2);
		oht->addInterval(interval3);
		//oht->addInterval(interval4);
		//oht->addInterval(interval5);
		
		oht->close(15);
	}
	
	void testRead()
	{
		delete iht;
		iht = new InHistoryTree();
		iht->setConfig(HistoryTreeConfig("./o.ht", 128, 3, 0));
		try{
			iht->open();
		}catch(...)
		{
			CPPUNIT_FAIL("Could not open tree for reading");
		}
		
		std::vector< IntervalSharedPtr > queryResult;
		
		
		for (timestamp_t i = 0; i < 8; i++)
		{
			try{
				queryResult = iht->query(i);
			}catch(TimeRangeEx& ex){
				CPPUNIT_FAIL("This is not possible!");
			}catch(...){
				CPPUNIT_FAIL("Something very wrong happened");
			}
			CPPUNIT_ASSERT(verify(queryResult, i));
		}
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
		iht->setConfig(ioConfig);
		try{
			ht->open();
		}catch(InvalidFormatEx& ex){
			CPPUNIT_FAIL("Invalid header");
		}catch(IOEx& ex){
			std::cerr << "2" << ex.what();
			CPPUNIT_FAIL("Could not open file for writing");
		}
	}
	
	void testBadExistingFile()
	{
		
	}
	
	void testHeader()
	{
		
	}
	
};

CPPUNIT_TEST_SUITE_REGISTRATION( TreeTest );
