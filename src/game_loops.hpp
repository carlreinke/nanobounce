#ifndef GAME_LOOPS_HPP
#define GAME_LOOPS_HPP

#include "highscore.hpp"
#include "level.hpp"
#include "level_set.hpp"
#include "loop.hpp"
#include "particle.hpp"

class LevelSetCongratsLoop : public Loop
{
public:
	LevelSetCongratsLoop( const LevelSet & );
	
	void handle_event( SDL_Event & );
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
private:
	const std::string set_name;
	
	std::list<Particle> particles;
};

class FireworkParticle : public Particle
{
public:
	FireworkParticle( Fixed x, Fixed y, const SDL_Color &color );
};

class LevelIntroLoop : public Loop
{
public:
	LevelIntroLoop( const Level &, const Highscore & );
	
	void handle_event( SDL_Event & );
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
private:
	const std::string level_name;
	const Highscore score;
	
	uint ticks;
};

class LevelCongratsLoop : public Loop
{
public:
	LevelCongratsLoop( const Level &, const Highscore & );
	
	void handle_event( SDL_Event & );
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
private:
	const std::string level_name;
	const Highscore score;
	
	std::list<Particle> particles;
	
	uint ticks;
};

class WooshParticle : public Particle
{
public:
	WooshParticle( Fixed x, Fixed y, const SDL_Color &color );
};

#endif // GAME_LOOPS_HPP
