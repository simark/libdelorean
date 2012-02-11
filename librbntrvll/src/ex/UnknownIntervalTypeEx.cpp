#include "UnknownIntervalTypeEx.hpp"

using namespace std;

UnknownIntervalTypeEx::UnknownIntervalTypeEx(void) {
	_msg = "unknown interval type";
}

const char* UnknownIntervalTypeEx::what() const throw() {
	return _msg.c_str();
}
