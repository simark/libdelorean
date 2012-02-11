#include "ExistingIntervalTypeEx.hpp"

using namespace std;

ExistingIntervalTypeEx::ExistingIntervalTypeEx(void) {
	_msg = "existing interval type";
}

const char* ExistingIntervalTypeEx::what() const throw() {
	return _msg.c_str();
}
