#ifndef GAME_LOOPS_HPP
#define GAME_LOOPS_HPP

#include "game_menus.hpp"
#include "highscore.hpp"
#include "level.hpp"
#include "level_pack.hpp"
#include "loop.hpp"
#include "particle.hpp"

class LevelPackCongratsLoop : public Loop
{
public:
	LevelPackCongratsLoop( const LevelPack & );
	
	void handle_event( SDL_Event & );
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
private:
	const std::string pack_name;
	
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
	LevelIntroLoop( const Level & );
	
	void handle_event( SDL_Event & );
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
private:
	const std::string level_name;
	
	uint ticks;
};

class LevelWonLoop : public SimpleMenu
{
public:
	LevelWonLoop( const Level &level, const Highscore &best_score, const Highscore &new_score );
	
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
private:
	const std::string level_name;
	const Highscore best_score;
	const Highscore new_score;
};

class LevelWonBestTimeLoop : public TextEntryMenu
{
public:
	LevelWonBestTimeLoop( const Level &level, const Highscore &new_score );
	
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
private:
	const std::string level_name;
	const Highscore new_score;
	
	std::list<Particle> particles;
	
	uint ticks;
};

class WooshParticle : public Particle
{
public:
	WooshParticle( Fixed x, Fixed y, const SDL_Color &color );
};

#endif // GAME_LOOPS_HPP
