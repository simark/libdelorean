#include "FactoryOverwritingEx.hpp"

using namespace std;

FactoryOverwritingEx::FactoryOverwritingEx(void) {
	_msg = "factory overwriting";
}

const char* FactoryOverwritingEx::what() const throw() {
	return _msg.c_str();
}
