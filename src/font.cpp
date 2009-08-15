#include "fixed.h"
#include "font.h"
#include "misc.h"

using namespace std;

ifstream font_stream("img/font.pgm"), meta_stream("img/font.meta");
Font font(font_stream, meta_stream);

Font::Font( std::istream &pgm, std::istream &is )
: char_positions(255), char_widths(255)
{
	load_pgm(pgm);
	load_char_widths(is);
}

void Font::load_pgm( istream &pgm )
{
	string type;
	pgm >> type;
	if (type != "P2")
	{
		cerr << "error: font loading failed" << endl;
		return;
	}
	
	int w = get_no_comments<int>(pgm),
	    h = get_no_comments<int>(pgm),
	    component_max = get_no_comments<int>(pgm);
	
	graymap.resize(h);
	
	int x = 0, y = 0;
	
	while (!pgm.eof() && y < h)
	{
		int g = get_no_comments<int>(pgm) * (component_max / (Fixed)255);
		graymap[y].push_back(g);
		
		if (++x == w)
		{
			x = 0;
			++y;
		}
	}
}

void Font::load_char_widths( istream &is )
{
	if (graymap.size() == 0)
		return;
	
	unsigned int x = 0;
	
	while (!is.eof() && x < graymap[0].size())
	{
		char c = get_no_comments<char>(is);
		int w = get_no_comments<int>(is);
		
		char_positions[c] = x;
		char_widths[c] = w - 1;
		
		if (c == 'N')
		{
			char_positions[' '] = graymap[0].size();
			char_widths[' '] = w / 2;
		}
		
		x += w;
	}
	
	assert(x == graymap[0].size());
}

void Font::blit( SDL_Surface *surface, int x, int y, const std::string &text, const Sprite &sprite, justifications justify, Uint8 alpha )
{
	switch (justify)
	{
	case center:
		x -= (width(text, sprite)) / 2;
		break;
	case right:
		x -= width(text, sprite);
		break;
	case left:
		break;
	}
	
	for (graymap_t::const_iterator y_line = graymap.begin(); y_line != graymap.end(); ++y_line)
	{
		int x_temp = x;
		
		for (string::const_iterator c = text.begin(); c != text.end(); ++c)
		{
			unsigned int g_x = char_positions[toupper(*c)];
			
			for (unsigned int x = 0; x < char_widths[toupper(*c)]; ++x)
			{
				if ((*y_line)[g_x + x] > 0)
					sprite.blit(surface, x_temp, y, alpha);
				
				x_temp += sprite.width();
			}
		}
		
		y += sprite.width();
	}
}

unsigned int Font::width( const std::string &text, const Sprite &sprite ) const
{
	unsigned int width = 0;
	
	for (unsigned int i = 0; i < text.length(); ++i)
		width += char_widths[toupper(text[i])] * sprite.width();
	
	return width;
}
