#ifndef FONT_HPP
#define FONT_HPP

#include "sprite.hpp"

class Font
{
public:
	Font( void );
	
	void load( const std::string &pgm_path, const std::string &meta_path );
	void load_pgm( std::istream &pgm );
	void load_char_widths( std::istream & );
	
	enum justifications
	{
		left,
		center,
		right
	};
	
	enum styles
	{
		normal,
		majuscule,
		minuscule
	};
	
	void blit( SDL_Surface *, int x, int y, const std::string &, const Sprite &, justifications = left, Uint8 alpha = SDL_ALPHA_OPAQUE );
	void blit( SDL_Surface *, int x, int y, std::string, const Sprite &, styles, justifications = left, Uint8 alpha = SDL_ALPHA_OPAQUE );
	
	uint height( const Sprite &sprite ) const { return graymap.size() * sprite.height(); };
	uint width( const std::string &, const Sprite & ) const;
	
private:
	void blit_char( SDL_Surface *, int x, int y, char, const Sprite &, Uint8 alpha );
	
	typedef std::vector< std::vector<unsigned char> > Graymap;
	
	Graymap graymap;
	std::vector<uint> char_positions, char_widths;
};

extern Font font;
extern std::map<int, Sprite> font_sprites;


#endif // FONT_HPP
