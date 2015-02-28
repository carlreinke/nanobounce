/*  nanobounce/src/game_loops.hpp
 *  
 *  Copyright 2015 Carl Reinke
 *  
 *  This program is non-commercial, open-source software; you can redistribute
 *  it and/or modify it under the terms of the MAME License as included along
 *  with this program.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  MAME License for more details.
 *  
 *  You should have received a copy of the MAME License along with this
 *  program; if not, see <http://www.intoorbit.org/licenses/MAME.txt>.
 */
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
	
protected:
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
	
protected:
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
	
protected:
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
	
protected:
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
