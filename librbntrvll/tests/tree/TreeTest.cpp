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

#include <ex/IOEx.hpp>
#include <ex/InvalidFormatEx.hpp>

#include <fstream>
#include <cstdio>

using namespace std;

class TreeTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TreeTest );
	
	CPPUNIT_TEST( testInexistentFile );
	
	CPPUNIT_TEST_SUITE_END();
	
private:
		
	OutHistoryTree* oht;
	InHistoryTree* iht;
	HistoryTree* ht;
	
public:
	void setUp()
	{			
		oht = new OutHistoryTree(HistoryTreeConfig());
		iht = new InHistoryTree(HistoryTreeConfig());
		ht = new HistoryTree(HistoryTreeConfig());
	}

	void tearDown() 
	{
		delete oht;
		delete iht;
		delete ht;
	}
	
	void testWrite()
	{
		
	}
	
	void testRead()
	{
		
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
