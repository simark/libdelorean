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
#include <IntervalCreator.hpp>
#undef protected
#undef private

#include <intervals/NullIntervalFactory.hpp>
#include <CustomIntervalFactory.hpp>
#include <CustomInterval.hpp>

#include <intervals/StringInterval.hpp>
#include <intervals/IntInterval.hpp>
#include <intervals/UIntInterval.hpp>
#include <intervals/FloatInterval.hpp>
#include <intervals/NullInterval.hpp>

#include <ex/TimeRangeEx.hpp>


class IntervalCreatorTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( IntervalCreatorTest );
	
	CPPUNIT_TEST( testRegister );
	CPPUNIT_TEST( testCreate );
	CPPUNIT_TEST( testCustomIntervalFactory );

	
	CPPUNIT_TEST_SUITE_END();
	
private:
	
	IntervalCreator* intervalCreator;
	
public:
	void setUp()
	{	
		intervalCreator = new IntervalCreator();
	}

	void tearDown() 
	{
		delete intervalCreator;
	}
	
	void testRegister()
	{
		IIntervalFactory* nullFactory1 = new NullIntervalFactory();
		IIntervalFactory* nullFactory2 = new NullIntervalFactory();
		intervalCreator->unregisterIntervalType(SIT_NULL);
		try{
			intervalCreator->registerIntervalType(SIT_NULL, nullFactory1);
		}catch(...){
			CPPUNIT_FAIL("Exception thrown when registering");
		}
		CPPUNIT_ASSERT_THROW(intervalCreator->registerIntervalType(SIT_NULL, nullFactory2), ExistingIntervalTypeEx);
		delete nullFactory2;
	}
	
	void testCreate()
	{
		AbstractInterval::SharedPtr i = intervalCreator->createIntervalFromType(SIT_STRING);
		CPPUNIT_ASSERT(i != 0);
		CPPUNIT_ASSERT(std::tr1::dynamic_pointer_cast<StringInterval>(i) != 0);
		
		i = intervalCreator->createIntervalFromType(SIT_INT32);
		CPPUNIT_ASSERT(i != 0);
		CPPUNIT_ASSERT(std::tr1::dynamic_pointer_cast<IntInterval>(i) != 0);
		
		i = intervalCreator->createIntervalFromType(SIT_UINT32);
		CPPUNIT_ASSERT(i != 0);
		CPPUNIT_ASSERT(std::tr1::dynamic_pointer_cast<UIntInterval>(i) != 0);
		
		i = intervalCreator->createIntervalFromType(SIT_FLOAT32);
		CPPUNIT_ASSERT(i != 0);
		CPPUNIT_ASSERT(std::tr1::dynamic_pointer_cast<FloatInterval>(i) != 0);
		
		i = intervalCreator->createIntervalFromType(SIT_NULL);
		CPPUNIT_ASSERT(i != 0);
		CPPUNIT_ASSERT(std::tr1::dynamic_pointer_cast<NullInterval>(i) != 0);
	}
	
	void testCustomIntervalFactory()
	{
		try{
			intervalCreator->registerIntervalType(5, new CustomIntervalFactory());
		}catch(...){
			CPPUNIT_FAIL("Exception thrown while registering");
		}
		
		AbstractInterval::SharedPtr i = intervalCreator->createIntervalFromType(5);
		CPPUNIT_ASSERT(i != 0);
		CPPUNIT_ASSERT(std::tr1::dynamic_pointer_cast<CustomInterval>(i) != 0);
		
		try{
			intervalCreator->unregisterIntervalType(5);
		}catch(...){
			CPPUNIT_FAIL("Exception thrown while unregistering");
		}
		
		CPPUNIT_ASSERT_THROW(i = intervalCreator->createIntervalFromType(5), UnknownIntervalTypeEx);
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION( IntervalCreatorTest );
