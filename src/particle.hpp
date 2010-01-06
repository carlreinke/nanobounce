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
	
	Fixed alpha, alpha_per_tick;
	
	typedef std::pair<int, int> coord;
	
	static const uint trail_max = 15;
	std::deque<coord> trail;
	
	boost::shared_ptr<Sprite> sprite;
	
	void draw( SDL_Surface *, int x_offset, int y_offset, Uint8 alpha ) const;
	
	void tick( void );
	
	static void tick_all( std::list<Particle> & );
	
protected:
	Particle( Fixed x, Fixed y, uint ticks_to_live, const boost::shared_ptr<Sprite> &sprite );
	
private:
	Particle( void );
};

class ExplosionParticle : public Particle
{
public:
	ExplosionParticle( Fixed x, Fixed y );
	
	static boost::shared_ptr<Sprite> sprite;
};

class DustParticle : public Particle
{
public:
	DustParticle( Fixed x, Fixed y );
	
	static boost::shared_ptr<Sprite> sprite;
};

class SparkParticle : public Particle
{
public:
	SparkParticle( Fixed x, Fixed y, Fixed x_vel, Fixed y_vel, const SDL_Color &color );
};

class FireworkParticle : public Particle
{
public:
	FireworkParticle( Fixed x, Fixed y, Fixed x_vel, Fixed y_vel, const SDL_Color &color );
};


#endif // PARTICLE_HPP
