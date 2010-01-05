#include "ball.hpp"
#include "particle.hpp"
#include "sdl_ext.hpp"

using namespace std;

Particle::Particle( Fixed x, Fixed y, uint ticks_to_live, const boost::shared_ptr<Sprite> &sprite )
: x(x), y(y),
  x_vel(0), y_vel(0),
  x_accel(0), y_accel(Ball::y_accel),
  x_term_vel(Ball::x_term_vel), y_term_vel(Ball::y_term_vel),
  ticks_to_live(ticks_to_live),
  alpha(SDL_ALPHA_OPAQUE), alpha_per_tick(0),
  sprite(sprite)
{
	/* nothing to do */
}

void Particle::draw( SDL_Surface *surface, int x_offset, int y_offset, Uint8 alpha ) const
{
	alpha = this->alpha * alpha >> 8;
	Uint8 alpha_trail = alpha / trail.size();
	
	for (deque<coord>::const_reverse_iterator i = trail.rbegin(); i != trail.rend(); ++i)
	{
		sprite->blit(surface, x_offset + i->first, y_offset + i->second, alpha);
		alpha -= alpha_trail;
	}
}

void Particle::tick( void )
{
	if (ticks_to_live > 0)
		--ticks_to_live;
	
	x_vel += x_accel;
	y_vel += y_accel;
	
	x_vel = min(max(-x_term_vel, x_vel), x_term_vel);
	y_vel = min(max(-y_term_vel, y_vel), y_term_vel);
	
	x += x_vel;
	y += y_vel;
	
	alpha = min(max(Fixed(SDL_ALPHA_TRANSPARENT), alpha + alpha_per_tick), Fixed(SDL_ALPHA_OPAQUE));
	
	trail.push_back(coord(x, y));
	if (trail.size() > trail_max)
		trail.pop_front();
}

void Particle::tick_all( list<Particle> &particles )
{
	for (list<Particle>::iterator i = particles.begin(); i != particles.end(); )
	{
		i->tick();
		
		if (i->ticks_to_live == 0)
			i = particles.erase(i);
		else
			++i;
	}
}

boost::shared_ptr<Sprite> ExplosionParticle::sprite(new Sprite(1, 1, SDL_Color_RGBA(255, 0, 0)));

ExplosionParticle::ExplosionParticle( Fixed x, Fixed y )
: Particle(x, y, rand() % 50 + 20, sprite)
{
	x_vel = Fixed(rand() % (1024 * 2) - 1024) / 1024;
	y_vel = Fixed(rand() % (1024 * 2) - 1024) / 1024;
	
	alpha = SDL_ALPHA_OPAQUE + static_cast<int>(ticks_to_live) - 50 - 20;
	alpha_per_tick = -alpha / static_cast<int>(ticks_to_live);
}

boost::shared_ptr<Sprite> DustParticle::sprite(new Sprite(1, 1, SDL_Color_RGBA(128, 128, 128)));

DustParticle::DustParticle( Fixed x, Fixed y )
: Particle(x, y, rand() % 60 + 20, sprite)
{
	x_vel = (Fixed(rand() % (1024 * 2) - 1024) / 1024) / 5;
	y_vel = (Fixed(rand() % 1024) / 1024) / 5;
	
	alpha = SDL_ALPHA_OPAQUE + static_cast<int>(ticks_to_live) - 60 - 20;
	alpha_per_tick = -alpha / static_cast<int>(ticks_to_live);
}

SparkParticle::SparkParticle( Fixed x, Fixed y, Fixed x_vel, Fixed y_vel, const SDL_Color &color )
: Particle(x, y, rand() % 20 + 20, boost::shared_ptr<Sprite>(new Sprite(1, 1, color)))
{
	this->x_vel = (Fixed(rand() % (1024 * 2) - 1024) / 1024) / 5 + x_vel;
	this->y_vel = (Fixed(rand() % (1024 * 2) - 1024) / 1024) / 5 + y_vel;
	
	alpha = SDL_ALPHA_OPAQUE + static_cast<int>(ticks_to_live) - 20 - 20;
	alpha_per_tick = -alpha / static_cast<int>(ticks_to_live);
}
