#include "misc.h"

using namespace std;

void getline( istream &is, istringstream &iss )
{
	string temp;
	getline(is, temp);
	iss.str(temp);
}
