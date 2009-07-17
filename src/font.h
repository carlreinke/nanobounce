#ifndef FONT_H
#define FONT_H

#include "sprite.h"

class Font
{
public:
	Font( std::istream &pgm, std::istream & );
	
	void load_pgm( std::istream &pgm );
	void load_char_widths( std::istream & );
	
	enum justifications
	{
		left,
		center,
		right
	};
	
	void blit( SDL_Surface *, int x, int y, const std::string &, const Sprite &, justifications = left, Uint8 alpha = SDL_ALPHA_OPAQUE );
	
	unsigned int height( const Sprite &sprite ) const { return graymap.size() * sprite.height(); };
	unsigned int width( const std::string &, const Sprite & ) const;
	
private:
	void blit_char( SDL_Surface *, int x, int y, char, const Sprite &, Uint8 alpha );
	
	typedef std::vector< std::vector<unsigned char> > graymap_t;
	
	graymap_t graymap;
	std::vector<unsigned int> char_positions, char_widths;
};

extern Font font;

#endif // FONT_H
