#include <tr1/memory>
#include <iostream>
#include <exception>
#include <cstring>

#include <IntIntervalFactory.hpp>
#include <IntInterval.hpp>
#include <IntervalCreator.hpp>
#include <StringInterval.hpp>

using namespace std;
using namespace std::tr1;

static void testInterval(void) {
	// create factory
	IIntervalFactory* factory = new IntIntervalFactory();
	
	// create interval
	IntervalSharedPtr interval = factory->create();
	delete factory;
	
	// set attributes
	interval->setStart(23)->setEnd(119)->setAttribute(11);
	int32_t val = -4123;
	interval->unserialize(NULL, &val);	
	
	// printing (debugging)
	cout << "printing:\n" << endl;
	cout << *interval << endl;
	
	// intersects
	cout << "intersects:\n" << endl;
	cout << "22 : " << interval->intersects(22) << endl <<
		"23 : " << interval->intersects(23) << endl <<
		"24 : " << interval->intersects(24) << endl <<
		"118 : " << interval->intersects(118) << endl <<
		"119 : " << interval->intersects(119) << endl <<
		"120 : " << interval->intersects(120) << endl;
}

static void testStringInterval(void) {
	char somebuf [] = "    here is a string";
	uint32_t len = strlen(somebuf) - 4;
	memcpy(somebuf, &len, 4);
	IntervalSharedPtr interval(new StringInterval());
	interval->unserialize(somebuf, NULL);
	interval->setInterval(13, 4657);
	interval->setAttribute(1422);
	cout << *interval << endl;
}

static void testIntervalCreator(void) {
	IntervalCreator creator;
	IntervalSharedPtr interval;
	
	try {
		creator.createIntervalFromType(1);
		creator.createIntervalFromType(5);
	} catch (const exception& ex) {
		cout << ex.what() << endl;
	}
}

int main(void) {
	cout << sizeof(float) << endl;
	testInterval();
	testIntervalCreator();
	testStringInterval();
	
	return 0;
}
