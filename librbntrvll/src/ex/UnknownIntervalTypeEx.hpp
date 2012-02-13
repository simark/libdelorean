#ifndef _UNKNOWNINTERVALTYPEEX_HPP
#define _UNKNOWNINTERVALTYPEEX_HPP

#include <string>
#include <stdexcept>

#include "../basic_types.h"

class UnknownIntervalTypeEx : public std::runtime_error
{
private:
	static std::string getMsg(interval_type_t type);
public:
	UnknownIntervalTypeEx(const std::string& msg) : runtime_error(msg) { }
	UnknownIntervalTypeEx(interval_type_t type) : runtime_error(getMsg(type)) { }
};


#endif // _UNKNOWNINTERVALTYPEEX_HPP

