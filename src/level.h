#ifndef LEVEL_H
#define LEVEL_H

#include "ball.h"
#include "block.h"
#include "SDL.h"

class Level
{
public:
	Level( void );
	Level( std::istream & );
	
	void load( std::istream & );
	
	void draw( SDL_Surface * ) const;
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
	Level( const Level & );
	bool operator=( const Level & );
	
	void check_collide( Ball &, Block & );
	
	int fade;
	
	std::vector<Ball> balls;
	std::vector<Block> blocks;
};

#endif
