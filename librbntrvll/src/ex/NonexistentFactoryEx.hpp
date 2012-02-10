#include <string>
#include <exception>

class NonexistentFactoryEx : public std::exception
{
public:
	NonexistentFactoryEx(void);
	const char* what() const throw();
	~NonexistentFactoryEx(void) throw() { }

private:
	std::string _msg;
};
