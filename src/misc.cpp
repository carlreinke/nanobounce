#include "misc.hpp"

void getline( std::istream &is, std::istringstream &iss )
{
	std::string temp;
	getline(is, temp);
	iss.str(temp);
}
