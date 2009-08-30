#ifndef MISC_HPP
#define MISC_HPP

#include "SDL.h"

#define COUNTOF(x) (sizeof(x) / sizeof(*x))

#define sqr(x) ((x) * (x))

void getline( std::istream &is, std::istringstream &iss );

template <class T>
T get_no_comments( std::istream &is )
{
	T temp;
	is >> temp;
	if (!is.fail())
		return temp;
		
	is.clear();
	while (is.peek() == '#' && !is.eof())
	{
		std::string dummy;
		getline(is, dummy);
	}
	
	is >> temp;
	return temp;
}

inline int SDL_FillRect( SDL_Surface *dst, SDL_Rect *dstrect, const SDL_Color &color )
{
	return SDL_FillRect(dst, dstrect, SDL_MapRGB(dst->format, color.r, color.g, color.b));
}

inline int SDL_FillRectA( SDL_Surface *dst, SDL_Rect *dstrect, const SDL_Color &color )
{
	return SDL_FillRect(dst, dstrect, SDL_MapRGBA(dst->format, color.r, color.g, color.b, color.unused));
}

#endif // MISC_HPP
