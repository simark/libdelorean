#include "NonexistentFactoryEx.hpp"

using namespace std;

NonexistentFactoryEx::NonexistentFactoryEx(void) {
	_msg = "nonexistent factory";
}

const char* NonexistentFactoryEx::what() const throw() {
	return _msg.c_str();
}
