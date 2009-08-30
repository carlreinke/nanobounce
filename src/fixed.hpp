#ifndef FIXED_HPP
#define FIXED_HPP

#ifdef HAS_FLOAT

typedef float Fixed;

#else // HAS_FLOAT

class Fixed
{
public:
	Fixed( void );
	Fixed( int );
	Fixed( float );
	
	operator int( void ) const;
	operator float( void ) const;
	
	friend bool operator<( const Fixed &, const Fixed & );
	friend bool operator>( const Fixed &, const Fixed & );
	friend bool operator<=( const Fixed &, const Fixed & );
	friend bool operator>=( const Fixed &, const Fixed & );
	friend bool operator==( const Fixed &, const Fixed & );
	friend bool operator!=( const Fixed &, const Fixed & );
	
	Fixed operator+( void ) const;
	Fixed operator-( void ) const;
	
	friend Fixed operator+( const Fixed &, const Fixed & );
	friend Fixed operator-( const Fixed &, const Fixed & );
	friend Fixed operator*( const Fixed &, const Fixed & );
	friend Fixed operator/( const Fixed &, const Fixed & );
	
	friend Fixed ceilf( const Fixed & );
	
private:
	static Fixed from_value( int );
	
	typedef long int Value;
	Value value;
	
	static const int bits = 16;
	static const Value mask = (1 << bits) - 1;
};

inline Fixed::Fixed( void )
{
	// good to go
}

inline Fixed::Fixed( int that )
{
	value = that * (1 << bits);
}

inline Fixed::Fixed( float that )
{
	value = (int)(that * (1 << bits));
}

inline Fixed Fixed::from_value( int value )
{
	Fixed temp;
	temp.value = value;
	return temp;
}

inline Fixed::operator int( void ) const
{
	return (int)value / (1 << bits);
}

inline Fixed::operator float( void ) const
{
	return (float)value / (1 << bits);
}

inline bool operator<( const Fixed &lhs, const Fixed &rhs )
{
	return lhs.value < rhs.value;
}

template <class T>
inline bool operator<( const T &lhs, const Fixed &rhs )
{
	return (Fixed)lhs < rhs;
}

template <class T>
inline bool operator<( const Fixed &lhs, const T &rhs )
{
	return lhs < (Fixed)rhs;
}

inline bool operator>( const Fixed &lhs, const Fixed &rhs )
{
	return lhs.value > rhs.value;
}

template <class T>
inline bool operator>( const T &lhs, const Fixed &rhs )
{
	return (Fixed)lhs > rhs;
}

template <class T>
inline bool operator>( const Fixed &lhs, const T &rhs )
{
	return lhs > (Fixed)rhs;
}

inline bool operator<=( const Fixed &lhs, const Fixed &rhs )
{
	return lhs.value <= rhs.value;
}

template <class T>
inline bool operator<=( const T &lhs, const Fixed &rhs )
{
	return (Fixed)lhs <= rhs;
}

template <class T>
inline bool operator<=( const Fixed &lhs, const T &rhs )
{
	return lhs <= (Fixed)rhs;
}

inline bool operator>=( const Fixed &lhs, const Fixed &rhs )
{
	return lhs.value >= rhs.value;
}

template <class T>
inline bool operator>=( const T &lhs, const Fixed &rhs )
{
	return (Fixed)lhs >= rhs;
}

template <class T>
inline bool operator>=( const Fixed &lhs, const T &rhs )
{
	return lhs >= (Fixed)rhs;
}

inline bool operator==( const Fixed &lhs, const Fixed &rhs )
{
	return lhs.value == rhs.value;
}

template <class T>
inline bool operator==( const T &lhs, const Fixed &rhs )
{
	return (Fixed)lhs == rhs;
}

template <class T>
inline bool operator==( const Fixed &lhs, const T &rhs )
{
	return lhs == (Fixed)rhs;
}

inline bool operator!=( const Fixed &lhs, const Fixed &rhs )
{
	return lhs.value != rhs.value;
}

template <class T>
inline bool operator!=( const T &lhs, const Fixed &rhs )
{
	return (Fixed)lhs != rhs;
}

template <class T>
inline bool operator!=( const Fixed &lhs, const T &rhs )
{
	return lhs != (Fixed)rhs;
}

inline Fixed Fixed::operator+( void ) const
{
	return *this;
}

inline Fixed Fixed::operator-( void ) const
{
	return from_value(-value);
}

inline Fixed operator+( const Fixed &lhs, const Fixed &rhs )
{
	return Fixed::from_value(lhs.value + rhs.value);
}

template <class T>
inline Fixed operator+( const T &lhs, const Fixed &rhs )
{
	return (Fixed)lhs + rhs;
}

template <class T>
inline Fixed operator+( const Fixed &lhs, const T &rhs )
{
	return lhs + (Fixed)rhs;
}

inline Fixed operator-( const Fixed &lhs, const Fixed &rhs )
{
	return Fixed::from_value(lhs.value - rhs.value);
}

template <class T>
inline Fixed operator-( const T &lhs, const Fixed &rhs )
{
	return (Fixed)lhs - rhs;
}

template <class T>
inline Fixed operator-( const Fixed &lhs, const T &rhs )
{
	return lhs - (Fixed)rhs;
}

inline Fixed operator*( const Fixed &lhs, const Fixed &rhs )
{
	return Fixed::from_value(((long long)lhs.value * rhs.value) / (1 << Fixed::bits));
}

template <class T>
inline Fixed operator*( const T &lhs, const Fixed &rhs )
{
	return (Fixed)lhs * rhs;
}

template <class T>
inline Fixed operator*( const Fixed &lhs, const T &rhs )
{
	return lhs * (Fixed)rhs;
}

inline Fixed operator/( const Fixed &lhs, const Fixed &rhs )
{
	return Fixed::from_value(((long long)lhs.value * (1 << Fixed::bits)) / rhs.value);
}

template <class T>
inline Fixed operator/( const T &lhs, const Fixed &rhs )
{
	return (Fixed)lhs / rhs;
}

template <class T>
inline Fixed operator/( const Fixed &lhs, const T &rhs )
{
	return lhs / (Fixed)rhs;
}

template <class T>
inline T & operator+=( T &lhs, const Fixed &rhs )
{
	return lhs = (Fixed)lhs + rhs;
}

template <class T>
inline T & operator-=( T &lhs, const Fixed &rhs )
{
	return lhs = (Fixed)lhs - rhs;
}

template <class T>
inline T & operator*=( T &lhs, const Fixed &rhs )
{
	return lhs = (Fixed)lhs * rhs;
}

template <class T>
inline T & operator/=( T &lhs, const Fixed &rhs )
{
	return lhs = (Fixed)lhs / rhs;
}

inline Fixed fabsf( const Fixed &that )
{
	return (that < 0) ? -that : that;
}

inline Fixed ceilf( const Fixed &that )
{
	return Fixed::from_value((that.value + that.mask) & ~that.mask);
}

inline std::ostream & operator<< ( std::ostream &lhs, const Fixed &rhs )
{
	return (lhs << (float)rhs);
}

#endif // HAS_FLOAT

#endif // FIXED_HPP
