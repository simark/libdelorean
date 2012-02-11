#ifndef _EXISTINGINTERVALTYPEEX_HPP
#define _EXISTINGINTERVALTYPEEX_HPP

#include <string>
#include <stdexcept>

class ExistingIntervalTypeEx : public std::runtime_error
{
public:
	ExistingIntervalTypeEx(const std::string& msg) : runtime_error(msg) { }
};

#endif // _EXISTINGINTERVALTYPEEX_HPP
