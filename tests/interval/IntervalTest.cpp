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
#include <intervals/StringInterval.hpp>
#include <intervals/IntInterval.hpp>
#include <intervals/UIntInterval.hpp>
#include <intervals/FloatInterval.hpp>
#include <intervals/NullInterval.hpp>
#undef protected
#undef private

#include <ex/TimeRangeEx.hpp>


class IntervalTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( IntervalTest );
	
	CPPUNIT_TEST( testStringIntervalProperties );
	CPPUNIT_TEST( testIntIntervalProperties );
	CPPUNIT_TEST( testUIntIntervalProperties );
	CPPUNIT_TEST( testFloatIntervalProperties );
	CPPUNIT_TEST( testNullIntervalProperties );
	CPPUNIT_TEST( testIntervalTypes );
	CPPUNIT_TEST( testIntervalVariableSize );
	CPPUNIT_TEST( testStringIntervalSerialize );
	CPPUNIT_TEST( testIntIntervalSerialize );
	CPPUNIT_TEST( testUIntIntervalSerialize );
	CPPUNIT_TEST( testFloatIntervalSerialize );
	CPPUNIT_TEST( testNullIntervalSerialize );
	CPPUNIT_TEST( testIntervalHeader );
	
	CPPUNIT_TEST_SUITE_END();
	
private:
		
	StringInterval* s_interval;
	IntInterval* i_interval;
	UIntInterval* u_interval;
	FloatInterval* f_interval;
	NullInterval* n_interval;
	
public:
	void setUp()
	{			
		s_interval = new StringInterval(12,15,1,"12-15-1");
		i_interval = new IntInterval(12,15,1,12151);
		u_interval = new UIntInterval(12,15,1,12151);
		f_interval = new FloatInterval(12,15,1,12.151);
		n_interval = new NullInterval(12,15,1);
	}

	void tearDown() 
	{
		delete s_interval;
		delete i_interval;
		delete u_interval;
		delete f_interval;
		delete n_interval;
	}
	
	void testStringIntervalProperties()
	{		
		CPPUNIT_ASSERT("12-15-1" == s_interval->getValue());
		CPPUNIT_ASSERT(12 == s_interval->getStart());
		CPPUNIT_ASSERT(15 == s_interval->getEnd());
		CPPUNIT_ASSERT(1 == s_interval->getAttribute());
		
	}
	
	void testIntIntervalProperties()
	{		
		CPPUNIT_ASSERT(12151 == i_interval->getValue());
		CPPUNIT_ASSERT(12 == i_interval->getStart());
		CPPUNIT_ASSERT(15 == i_interval->getEnd());
		CPPUNIT_ASSERT(1 == i_interval->getAttribute());
		
	}
	
	void testUIntIntervalProperties()
	{		
		CPPUNIT_ASSERT(12151 == u_interval->getValue());
		CPPUNIT_ASSERT(12 == u_interval->getStart());
		CPPUNIT_ASSERT(15 == u_interval->getEnd());
		CPPUNIT_ASSERT(1 == u_interval->getAttribute());
		
	}
	
	void testFloatIntervalProperties()
	{		
		CPPUNIT_ASSERT(fabs(12.151 - f_interval->getValue()) < 0.00001);
		CPPUNIT_ASSERT(12 == f_interval->getStart());
		CPPUNIT_ASSERT(15 == f_interval->getEnd());
		CPPUNIT_ASSERT(1 == f_interval->getAttribute());
		
	}
	
	void testNullIntervalProperties()
	{		
		CPPUNIT_ASSERT(12 == f_interval->getStart());
		CPPUNIT_ASSERT(15 == f_interval->getEnd());
		CPPUNIT_ASSERT(1 == f_interval->getAttribute());
		
	}
	
	void testIntervalTypes()
	{
		CPPUNIT_ASSERT(s_interval->getType() == (interval_type_t)1);
		CPPUNIT_ASSERT(i_interval->getType() == (interval_type_t)0);
		CPPUNIT_ASSERT(u_interval->getType() == (interval_type_t)2);
		CPPUNIT_ASSERT(f_interval->getType() == (interval_type_t)3);
		CPPUNIT_ASSERT(n_interval->getType() == (interval_type_t)-1);
		
		CPPUNIT_ASSERT(StringInterval::type == (interval_type_t)1);
		CPPUNIT_ASSERT(IntInterval::type == (interval_type_t)0);
		CPPUNIT_ASSERT(UIntInterval::type == (interval_type_t)2);
		CPPUNIT_ASSERT(FloatInterval::type == (interval_type_t)3);
		CPPUNIT_ASSERT(NullInterval::type == (interval_type_t)-1);
	}
	
	void testIntervalVariableSize()
	{
		CPPUNIT_ASSERT(s_interval->getVariableValueSize() == 2+7);
		CPPUNIT_ASSERT(i_interval->getVariableValueSize() == 0);
		CPPUNIT_ASSERT(u_interval->getVariableValueSize() == 0);
		CPPUNIT_ASSERT(f_interval->getVariableValueSize() == 0);
		CPPUNIT_ASSERT(n_interval->getVariableValueSize() == 0);
	}
	
	void testStringIntervalSerialize()
	{
		unsigned char constant[4];
		unsigned char variable[256];
		unsigned char expected[256];
		memset(constant, 0, 4);
		memset(variable, 0, 256);
		memset(expected, 0, 256);
		expected[0] = 9;
		expected[1] = '1';
		expected[2] = '2';
		expected[3] = '-';
		expected[4] = '1';
		expected[5] = '5';
		expected[6] = '-';
		expected[7] = '1';
		expected[8] = 0;
		
		s_interval->serializeValues(variable, constant);
		
		CPPUNIT_ASSERT( memcmp(variable, expected, 256) == 0);
	}
	
	void testIntIntervalSerialize()
	{
		unsigned char constant[4];
		unsigned char variable[256];
		unsigned char expected[256];
		memset(constant, 0, 4);
		memset(variable, 0, 256);
		memset(expected, 0, 256);
		
		i_interval->serializeValues(variable, constant);
		
		CPPUNIT_ASSERT( *(int*)constant == 12151);
		CPPUNIT_ASSERT( memcmp(variable, expected, 256) == 0);
	}
	
	void testUIntIntervalSerialize()
	{
		unsigned char constant[4];
		unsigned char variable[256];
		unsigned char expected[256];
		memset(constant, 0, 4);
		memset(variable, 0, 256);
		memset(expected, 0, 256);
		
		u_interval->serializeValues(variable, constant);
		
		CPPUNIT_ASSERT( *(int*)constant == 12151);
		CPPUNIT_ASSERT( memcmp(variable, expected, 256) == 0);
	}
	
	void testFloatIntervalSerialize()
	{
		unsigned char constant[4];
		unsigned char variable[256];
		unsigned char expected[256];
		memset(constant, 0, 4);
		memset(variable, 0, 256);
		memset(expected, 0, 256);
		
		f_interval->serializeValues(variable, constant);
		
		CPPUNIT_ASSERT( fabs(*(float*)constant - 12.151) < 0.00001);
		CPPUNIT_ASSERT( memcmp(variable, expected, 256) == 0);
	}
	
	void testNullIntervalSerialize()
	{
		unsigned char constant[4];
		unsigned char variable[256];
		unsigned char expected[256];
		memset(constant, 0, 4);
		memset(variable, 0, 256);
		memset(expected, 0, 256);
		
		n_interval->serializeValues(variable, constant);
		
		CPPUNIT_ASSERT( *(int*)constant == 0);
		CPPUNIT_ASSERT( memcmp(variable, expected, 256) == 0);
	}
	
	void testIntervalHeader()
	{	
		unsigned char buffer[256];
		unsigned char expected[256];
		unsigned char variable[256];
		memset(buffer, 0, 256);
		memset(expected, 0, 256);
		
		// TEST STRING
		
		*((uint64_t*) &expected[0]) = 12;
		*((uint64_t*) &expected[8]) = 15;
		*((uint32_t*) &expected[16]) = 1;
		expected[20] = (interval_type_t)1;
		
		s_interval->serialize(variable, buffer);
		
		CPPUNIT_ASSERT( memcmp(buffer, expected, 256) == 0);
		
		//TEST INT
		
		memset(buffer, 0, 256);
		memset(expected, 0, 256);
		
		*((uint64_t*) &expected[0]) = 12;
		*((uint64_t*) &expected[8]) = 15;
		*((uint32_t*) &expected[16]) = 1;
		expected[20] = (interval_type_t)0;
		*((uint32_t*) &expected[21]) = 12151;
		
		i_interval->serialize(variable, buffer);
		
		CPPUNIT_ASSERT( memcmp(buffer, expected, 256) == 0);
		
		//TEST UINT
		
		memset(buffer, 0, 256);
		memset(expected, 0, 256);
		
		*((uint64_t*) &expected[0]) = 12;
		*((uint64_t*) &expected[8]) = 15;
		*((uint32_t*) &expected[16]) = 1;
		expected[20] = (interval_type_t)2;
		*((uint32_t*) &expected[21]) = 12151;
		
		u_interval->serialize(variable, buffer);
		
		CPPUNIT_ASSERT( memcmp(buffer, expected, 256) == 0);
		
		//TEST FLOAT
		
		memset(buffer, 0, 256);
		memset(expected, 0, 256);
		
		*((uint64_t*) &expected[0]) = 12;
		*((uint64_t*) &expected[8]) = 15;
		*((uint32_t*) &expected[16]) = 1;
		expected[20] = (interval_type_t)3;
		*((float*) &expected[21]) = 12.151;
		
		f_interval->serialize(variable, buffer);
		
		CPPUNIT_ASSERT( memcmp(buffer, expected, 21) == 0);
		CPPUNIT_ASSERT( fabs(*(float*)&buffer[21] - 12.151) < 0.00001);
		
		//TEST NULL
		
		memset(buffer, 0, 256);
		memset(expected, 0, 256);
		
		*((uint64_t*) &expected[0]) = 12;
		*((uint64_t*) &expected[8]) = 15;
		*((uint32_t*) &expected[16]) = 1;
		expected[20] = (interval_type_t)-1;
		
		n_interval->serialize(variable, buffer);
		
		CPPUNIT_ASSERT( memcmp(buffer, expected, 21) == 0);
	}

	
};

CPPUNIT_TEST_SUITE_REGISTRATION( IntervalTest );
