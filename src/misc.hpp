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

// this is only useful for one-to-one maps
template <typename K, typename V>
std::map<V, K> flip( const std::map<K, V> &map )
{
	std::map<V, K> temp;
	
	for (typename std::map<K, V>::const_iterator i = map.begin(); i != map.end(); ++i)
		temp[i->second] = i->first;
	
	return temp;
}

#endif // MISC_HPP
