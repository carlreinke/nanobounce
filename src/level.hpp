#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "ball.hpp"
#include "block.hpp"
#include "SDL.h"

class Level
{
public:
	Level( void );
	Level( std::istream & );
	
	void load( std::istream & );
	
	void draw( SDL_Surface *, int x_offset, int y_offset ) const;
	void update( int ball_x_direction );
	
	std::string name;
	
	bool done;
	enum states
	{
		none,
		won,
		lost,
		quit,
		
		load_failed
	}
	state;
	
	static const int width = 320, height = 240;
	
private:
	void check_collide( Ball &, Block & );
	
	int fade;
	
	std::vector<Ball> balls;
	std::vector<Block> blocks;
};

#endif // LEVEL_HPP
