#ifndef LEVEL_SET_HPP
#define LEVEL_SET_HPP

#include "level.hpp"
#include "loop.hpp"
#include "particle.hpp"

class LevelSet
{
public:
	LevelSet( const std::string &directory );
	bool invalid( void ) const { return !valid; }
	bool operator<( const LevelSet &that ) const { return this->name < that.name; }
	
	void play( SDL_Surface * );
	
	std::string name, author;
	
private:
	bool valid;
	
	std::string directory;
};

class CongratsLoop : public Loop
{
public:
	CongratsLoop( const std::string &pack_name );
	
	void handle_event( SDL_Event & );
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
private:
	const std::string pack_name;
	
	std::list<Particle> particles;
};

void play_pack( SDL_Surface *surface, const std::string &directory );

void level_screen( SDL_Surface *surface, const Level &level, const Highscore &highscore );

#endif // LEVEL_SET_HPP
