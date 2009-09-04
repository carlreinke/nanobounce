#ifndef MISC_HPP
#define MISC_HPP

#define COUNTOF(x) (sizeof(x) / sizeof(*x))

#define sqr(x) ((x) * (x))

void getline( std::istream &is, std::istringstream &iss );

template <class T>
T get_no_comments( std::istream &is )
{
	T temp;
	is >> temp;
	if (is.good())
		return temp;
		
	is.clear();
	while (is.peek() == '#' && is.good())
	{
		std::string dummy;
		getline(is, dummy);
	}
	
	is >> temp;
	return temp;
}

#endif // MISC_HPP
