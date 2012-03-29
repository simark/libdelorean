#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include <istream>
#include <ostream>
#include <iostream>
#include <string>

#define protected public
#define private public
#include <AbstractNode.hpp>
#include <LeafNode.hpp>
#include <CoreNode.hpp>
#undef protected
#undef private
#include <intervals/IntInterval.hpp>
#include <intervals/StringInterval.hpp>

#include <ex/TimeRangeEx.hpp>


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
	LeafNode::SharedPtr leafNode;
	CoreNode::SharedPtr coreNode;
	
	AbstractNode::SharedPtr thirdNode;
	
	AbstractInterval::SharedPtr interval_core_1;
	AbstractInterval::SharedPtr interval_leaf_1;
	AbstractInterval::SharedPtr interval_leaf_2;
	AbstractInterval::SharedPtr interval_leaf_3;
	
public:
	void setUp()
	{	
		// some config
		// with those parameters, only 3 IntInterval fit into a leaf node and
		// 1 into a core node
		config = HistoryTreeConfig("", 128, 4, 0); //size 128 bytes, 4 children
		
		leafNode.reset(new LeafNode(config, 1, 2, 10));
		coreNode.reset(new CoreNode(config, 2, -1, 5));
		coreNode->linkNewChild(leafNode);
		
		interval_core_1.reset(new IntInterval(12, 14, 1, 0));
		interval_leaf_1.reset(new IntInterval(15, 19, 4, 14));
		interval_leaf_2.reset(new IntInterval(13, 17, 2, 12));
		interval_leaf_3.reset(new StringInterval(14, 18, 3, "treize"));
		
		coreNode->addInterval(interval_core_1);
		
		leafNode->addInterval(interval_leaf_1);
		leafNode->addInterval(interval_leaf_2);
		leafNode->addInterval(interval_leaf_3);
		
		coreNode->close(20);
		leafNode->close(20);
		
		thirdNode.reset(new LeafNode(config, 3, 2, 15));
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
		CPPUNIT_ASSERT_THROW(thirdNode->addInterval(AbstractInterval::SharedPtr(new IntInterval(12, 14, 1, 0))), TimeRangeEx);
	}
	
	void testIntervalSort()
	{
		IntervalContainer::const_iterator it = leafNode->_intervals.begin();
		while(it != leafNode->_intervals.end())
		{
			IntervalContainer::const_iterator nextIt = it;
			nextIt++;
			if(nextIt != leafNode->_intervals.end())
			{
				CPPUNIT_ASSERT((*it)->getEnd() <= (*(nextIt))->getEnd());
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
		CPPUNIT_ASSERT(coreNode->getFreeSpace() == 128-(42+16+32)-(1*25)
		-interval_core_1->getVariableValueSize());
		CPPUNIT_ASSERT(leafNode->getFreeSpace() == 128-(34)      -(3*25)
		-interval_leaf_1->getVariableValueSize()-interval_leaf_2->getVariableValueSize()-interval_leaf_3->getVariableValueSize());
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
		CPPUNIT_ASSERT(leafNode->_variableSectionOffset == data_offset);
		
		bool done;
		s.read((char*)&done, 1);
		CPPUNIT_ASSERT(1 == done);
	}	
};

CPPUNIT_TEST_SUITE_REGISTRATION( NodeTest );
