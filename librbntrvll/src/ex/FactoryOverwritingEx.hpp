#include <string>
#include <exception>

class FactoryOverwritingEx : public std::exception
{
public:
	FactoryOverwritingEx(void);
	const char* what() const throw();
	~FactoryOverwritingEx(void) throw() { }

private:
	std::string _msg;
};
