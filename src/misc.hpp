/*  nanobounce/src/misc.hpp
 *  
 *  Copyright 2015 Carl Reinke
 *  
 *  This program is non-commercial, open-source software; you can redistribute
 *  it and/or modify it under the terms of the MAME License as included along
 *  with this program.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  MAME License for more details.
 *  
 *  You should have received a copy of the MAME License along with this
 *  program; if not, see <http://www.intoorbit.org/licenses/MAME.txt>.
 */
#ifndef MISC_HPP
#define MISC_HPP

#define COUNTOF(x) (sizeof(x) / sizeof(*x))

#define sqr(x) ((x) * (x))

template <typename T>
static inline T make_frac( const T &a, const T &b )
{
	return (a / b);
}

typedef unsigned int uint;

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
		std::getline(is, dummy);
	}
	
	is >> temp;
	return temp;
}

template <typename T, typename U>
static inline typename boost::bimap<T, U>::value_type make_bipair( T x, U y )
{
	return typename boost::bimap<T, U>::value_type(x, y);
}

#endif // MISC_HPP
