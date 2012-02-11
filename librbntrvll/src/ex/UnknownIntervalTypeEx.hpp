#ifndef _UNKNOWNINTERVALTYPE_HPP
#define _UNKNOWNINTERVALTYPE_HPP

#include <string>
#include <exception>

class UnknownIntervalTypeEx : public std::exception
{
public:
	UnknownIntervalTypeEx(void);
	const char* what() const throw();
	~UnknownIntervalTypeEx(void) throw() { }

private:
	std::string _msg;
};

#endif // _UNKNOWNINTERVALTYPE_HPP

