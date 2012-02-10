#ifndef _IPRINTABLE_HPP
#define _IPRINTABLE_HPP

#include <string>

class IPrintable
{
	public:
	virtual std::string toString(void) const = 0;
};

#endif // _IPRINTABLE_HPP
