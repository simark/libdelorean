#include <tr1/memory>
#include <iostream>

#include <IntInterval.hpp>

using namespace std;
using namespace std::tr1;

static void testInterval(void) {
	// printing (mostly for debugging)
	shared_ptr<IntInterval> int_interval(new IntInterval(23, 119, 1, -76));
	cout << "printing:\n" << endl;
	cout << *int_interval << endl;
	
	// intersects
	cout << "intersects:\n" << endl;
	cout << "22 : " << int_interval->intersects(22) << endl <<
		"23 : " << int_interval->intersects(23) << endl <<
		"24 : " << int_interval->intersects(24) << endl <<
		"118 : " << int_interval->intersects(118) << endl <<
		"119 : " << int_interval->intersects(119) << endl <<
		"120 : " << int_interval->intersects(120) << endl;
}

int main(void) {
	testInterval();	
	
	return 0;
}
