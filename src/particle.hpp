#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "fixed.hpp"
#include "sprite.hpp"

class Particle
{
public:
	Fixed x, y;
	Fixed x_vel, y_vel;
	Fixed x_accel, y_accel;
	
	Fixed x_term_vel, y_term_vel;
	
	uint ticks_to_live;
	
	std::vector<Sprite>::const_iterator sprite, last_sprite;
	uint next_sprite;
	
	void draw( SDL_Surface *, int x_offset, int y_offset, Uint8 alpha ) const;
	
	void tick( void );
	
	static void tick_all( std::list<Particle> & );
	
protected:
	Particle( Fixed x, Fixed y, uint ticks_to_live );
	
private:
	Particle( void );
};

class ExplosionParticle : public Particle
{
public:
	ExplosionParticle( Fixed x, Fixed y );
	
	static std::vector<Sprite> sprites;
};

class DustParticle : public Particle
{
public:
	DustParticle( Fixed x, Fixed y );
	
	static std::vector<Sprite> sprites;
};

#endif // PARTICLE_HPP
