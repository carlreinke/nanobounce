/*  nanobounce/src/particle.hpp
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
#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "fixed.hpp"
#include "sdl_ext.hpp"
#include "video/sprite.hpp"

class Particle
{
public:
	Fixed x, y;
	Fixed x_vel, y_vel;
	Fixed x_accel, y_accel;
	
	Fixed term_vel;
	
	uint ticks_to_live;
	
	Fixed alpha, alpha_per_tick;
	
	struct Coord
	{
		Coord( int x, int y ) : x(x), y(y) { }
		
		int x;
		int y;
	};
	
	const static size_t trail_max = 15;
	boost::circular_buffer<Coord> trail;
	
	SDL_Color color;
	
	void draw( SDL_Surface *, int x_offset, int y_offset, Uint8 alpha ) const;
	
	void tick( void );
	
	static void tick_all( std::list<Particle> & );
	
protected:
	Particle( Fixed x, Fixed y, uint ticks_to_live, const SDL_Color & );
	
	static const Fixed constant, usual_y_accel, usual_term_vel;
	
private:
	Particle( void );
};

class ExplosionParticle : public Particle
{
public:
	ExplosionParticle( Fixed x, Fixed y, const SDL_Color &color = SDL_Color_RGBA(255, 0, 0) );
};

class DustParticle : public Particle
{
public:
	DustParticle( Fixed x, Fixed y, const SDL_Color &color = SDL_Color_RGBA(128, 128, 128) );
};

class StarDustParticle : public Particle
{
public:
	StarDustParticle( Fixed x, Fixed y, const SDL_Color &color = SDL_Color_RGBA(255, 255, 0) );
};

class SparkParticle : public Particle
{
public:
	SparkParticle( Fixed x, Fixed y, Fixed x_vel, Fixed y_vel, const SDL_Color &color );
};

#endif // PARTICLE_HPP
