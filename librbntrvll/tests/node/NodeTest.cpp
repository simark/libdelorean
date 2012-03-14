#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include <istream>
#include <ostream>
#include <iostream>
#include <string>

#define protected public
#define private public
#include <HistoryTreeNode.hpp>
#include <HistoryTreeLeafNode.hpp>
#include <HistoryTreeCoreNode.hpp>
#undef protected
#undef private
#include <IntInterval.hpp>
#include <StringInterval.hpp>


class NodeTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( NodeTest );
	
	CPPUNIT_TEST( testProperties );
	CPPUNIT_TEST( testIntervalInsert );
	CPPUNIT_TEST( testIntervalSort );
	CPPUNIT_TEST( testHeaderSize );
	CPPUNIT_TEST( testFreeSpace );
	CPPUNIT_TEST( testIntervalSearch );
	CPPUNIT_TEST( testCoreSerialize );
	CPPUNIT_TEST( testLeafSerialize );
	
	CPPUNIT_TEST_SUITE_END();
	
private:
	HistoryTreeConfig config;
	HistoryTreeLeafNodeSharedPtr leafNode;
	HistoryTreeCoreNodeSharedPtr coreNode;
	
public:
	void setUp()
	{	
		// some config
		// with those parameters, only 3 IntInterval fit into a leaf node and
		// 1 into a core node
		config = HistoryTreeConfig("", 128, 4, 0); //size 128 bytes, 4 children
		
		leafNode.reset(new HistoryTreeLeafNode(config, 1, 2, 10));
		coreNode.reset(new HistoryTreeCoreNode(config, 2, -1, 5));
		coreNode->linkNewChild(leafNode);
		
		coreNode->addInterval(IntervalSharedPtr(new IntInterval(12, 14, 1, 0)));
		
		leafNode->addInterval(IntervalSharedPtr(new IntInterval(15, 19, 4, 14)));
		leafNode->addInterval(IntervalSharedPtr(new IntInterval(13, 17, 2, 12)));
		leafNode->addInterval(IntervalSharedPtr(new StringInterval(14, 18, 3, "treize")));
		
		coreNode->close(20);
		leafNode->close(20);
	}

	void tearDown() 
	{
		
	}
	
	void testProperties()
	{		
		CPPUNIT_ASSERT(10 == leafNode->getStart());
		CPPUNIT_ASSERT(5 == coreNode->getStart());
		
		CPPUNIT_ASSERT(20  ==  leafNode->getEnd());
		CPPUNIT_ASSERT(20 == coreNode->getEnd());
		
		CPPUNIT_ASSERT(1 == leafNode->getSequenceNumber());
		CPPUNIT_ASSERT(2 == coreNode->getSequenceNumber());
		
		CPPUNIT_ASSERT(2 == leafNode->getParentSequenceNumber());
		CPPUNIT_ASSERT(-1 == coreNode->getParentSequenceNumber());
		
		//FIXME : These next ones are incomplete
		
		CPPUNIT_ASSERT(true == leafNode->isDone());
		CPPUNIT_ASSERT(true == coreNode->isDone());
	}
	
	void testIntervalInsert()
	{
		//CPPUNIT_ASSERT_NO_THROW(coreNode->addInterval(IntervalSharedPtr(new IntInterval(12, 14, 1, 0))));
		
		//CPPUNIT_ASSERT_NO_THROW(leafNode->addInterval(IntervalSharedPtr(new IntInterval(15, 19, 4, 14))));
		//CPPUNIT_ASSERT_NO_THROW(leafNode->addInterval(IntervalSharedPtr(new IntInterval(13, 17, 2, 12))));
		//CPPUNIT_ASSERT_NO_THROW(leafNode->addInterval(IntervalSharedPtr(new StringInterval(14, 18, 3, "treize"))));
		
		
		//coreNode->addInterval(IntervalSharedPtr(new IntInterval(12, 14, 1, 0)));
		
		//leafNode->addInterval(IntervalSharedPtr(new IntInterval(15, 19, 4, 14)));
		//leafNode->addInterval(IntervalSharedPtr(new IntInterval(13, 17, 2, 12)));
		//leafNode->addInterval(IntervalSharedPtr(new StringInterval(14, 18, 3, "treize")));
	}
	
	void testIntervalSort()
	{
		leafNode->close(20); //This will sort the intervals
		std::vector<IntervalSharedPtr>::const_iterator it = leafNode->_intervals.begin();
		while(it != leafNode->_intervals.end())
		{
			if(it != leafNode->_intervals.end()-1)
			{
				CPPUNIT_ASSERT((*it)->getEnd() < (*(it+1))->getEnd());
			}
			it++;
		}
	}

	void testHeaderSize()
	{
		CPPUNIT_ASSERT(coreNode->getTotalHeaderSize() == 42+16+32);
		CPPUNIT_ASSERT(leafNode->getTotalHeaderSize() == 34);
	}
	
	void testFreeSpace()
	{
		CPPUNIT_ASSERT(coreNode->getFreeSpace() == 128-(42+16+32)-(1*25));
		CPPUNIT_ASSERT(leafNode->getFreeSpace() == 128-(34)      -(3*25)-8);
	}
	
	void testIntervalSearch()
	{
		IntervalContainer::iterator it = leafNode->_intervals.begin();
		CPPUNIT_ASSERT(leafNode->getStartIndexFor(17) == it++);
		CPPUNIT_ASSERT(leafNode->getStartIndexFor(18) == it++);
		CPPUNIT_ASSERT(leafNode->getStartIndexFor(19) == it++);
		CPPUNIT_ASSERT(leafNode->getStartIndexFor(20) == it++);
	}
	
	void testCoreSerialize()
	{
		std::stringbuf buf;
		std::iostream s(&buf);
		std::ostream& os(s);
		coreNode->serialize(os);
		
		node_type_t type;
		s.read((char*)&type, 1);
		CPPUNIT_ASSERT_MESSAGE("type serialize failed", 1 == type);
		
		timestamp_t start;
		s.read((char*)&start, 8);
		CPPUNIT_ASSERT_MESSAGE("start serialize failed", 5 == start);
		
		timestamp_t end;
		s.read((char*)&end, 8);
		CPPUNIT_ASSERT_MESSAGE("end serialize failed", 20 == end);
		
		seq_number_t seq;
		s.read((char*)&seq, 4);
		CPPUNIT_ASSERT_MESSAGE("seq serialize failed", 2 == seq);
		
		seq_number_t parent_seq;
		s.read((char*)&parent_seq, 4);
		CPPUNIT_ASSERT_MESSAGE("parent seq serialize failed", -1 == parent_seq);
		
		int interval_count;
		s.read((char*)&interval_count, 4);
		CPPUNIT_ASSERT_MESSAGE("interval count serialize failed", 1 == interval_count);
		
		int data_offset;
		s.read((char*)&data_offset, 4);
		CPPUNIT_ASSERT_MESSAGE("data offset serialize failed", 128 == data_offset);
		
		bool done;
		s.read((char*)&done, 1);
		CPPUNIT_ASSERT_MESSAGE("done serialize failed", 1 == done);
		
		int extended;
		s.read((char*)&extended, 4);
		//CPPUNIT_ASSERT(0 == extended);
		
		int children_count;
		s.read((char*)&children_count, 4);
		CPPUNIT_ASSERT_MESSAGE("children count serialize failed", 1 == children_count);
		
		int child_seq;
		s.read((char*)&child_seq, 4);
		CPPUNIT_ASSERT_MESSAGE("children count serialize failed", 1 == child_seq);
		
		s.ignore(12);
		
		timestamp_t child_start;
		s.read((char*)&child_start, 8);
		CPPUNIT_ASSERT_MESSAGE("child start serialize failed", 10 == child_start);
	}
	
	void testLeafSerialize()
	{
		std::stringbuf buf;
		std::iostream s(&buf);
		std::ostream& os(s);
		leafNode->serialize(os);
		
		node_type_t type;
		s.read((char*)&type, 1);
		CPPUNIT_ASSERT(2 == type);
		
		timestamp_t start;
		s.read((char*)&start, 8);
		CPPUNIT_ASSERT(10 == start);
		
		timestamp_t end;
		s.read((char*)&end, 8);
		CPPUNIT_ASSERT(20 == end);
		
		seq_number_t seq;
		s.read((char*)&seq, 4);
		CPPUNIT_ASSERT(1 == seq);
		
		seq_number_t parent_seq;
		s.read((char*)&parent_seq, 4);
		CPPUNIT_ASSERT(2 == parent_seq);
		
		int interval_count;
		s.read((char*)&interval_count, 4);
		CPPUNIT_ASSERT(3 == interval_count);		
		
		int data_offset;
		s.read((char*)&data_offset, 4);
		CPPUNIT_ASSERT(120 == data_offset);
		
		bool done;
		s.read((char*)&done, 1);
		CPPUNIT_ASSERT(1 == done);
	}	
};

CPPUNIT_TEST_SUITE_REGISTRATION( NodeTest );
