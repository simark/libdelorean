#ifndef _EXISTINGINTERVALTYPEEX_HPP
#define _EXISTINGINTERVALTYPEEX_HPP

#include <string>
#include <exception>

class ExistingIntervalTypeEx : public std::exception
{
public:
	ExistingIntervalTypeEx(void);
	const char* what() const throw();
	~ExistingIntervalTypeEx(void) throw() { }

private:
	std::string _msg;
};

#endif // _EXISTINGINTERVALTYPEEX_HPP
