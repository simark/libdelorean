#include <string>
#include <sstream>

#include "UnknownIntervalTypeEx.hpp"
#include "../basic_types.h"

using namespace std;

std::string UnknownIntervalTypeEx::getMsg(interval_type_t type) {
	ostringstream oss;
	oss << "unknown interval type " << (unsigned int) type;
	
	return oss.str();
}
