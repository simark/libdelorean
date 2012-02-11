#ifndef _UNKNOWNINTERVALTYPE_HPP
#define _UNKNOWNINTERVALTYPE_HPP

#include <string>
#include <stdexcept>

class UnknownIntervalTypeEx : public std::runtime_error
{
public:
	UnknownIntervalTypeEx(const std::string& msg) : runtime_error(msg) { }
};


#endif // _UNKNOWNINTERVALTYPE_HPP

