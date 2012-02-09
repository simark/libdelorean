#ifndef _IPRINTABLE_H
#define _IPRINTABLE_H

#include <string>

class IPrintable
{
	public:
	virtual std::string toString(void) const = 0;
};

#endif // _IPRINTABLE_H
