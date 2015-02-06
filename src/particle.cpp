#include "block.hpp"
#include "particle.hpp"

using std::max;
using std::min;

const Fixed Particle::constant = 1;

// terminal velocity and acceleration
const Fixed Particle::usual_term_vel = constant;
const Fixed Particle::usual_y_accel  = sqr(-usual_term_vel) / (2 * (1.5f * Block::height));

Particle::Particle( Fixed x, Fixed y, uint ticks_to_live, const SDL_Color &color )
: x(x), y(y),
  x_vel(0), y_vel(0),
  x_accel(0), y_accel(usual_y_accel),
  term_vel(usual_term_vel),
  ticks_to_live(ticks_to_live),
  alpha(SDL_ALPHA_OPAQUE), alpha_per_tick(0),
  color(color)
{
	/* nothing to do */
}

void Particle::draw( SDL_Surface *surface, int x_offset, int y_offset, Uint8 alpha ) const
{
	alpha = static_cast<int>(this->alpha) * alpha / 256;
	Uint8 alpha_trail = alpha / trail.size();
	
	for (std::deque<coord>::const_reverse_iterator i = trail.rbegin(); i != trail.rend(); ++i)
	{
		SDL_SetPixelA(surface, x_offset + i->first, y_offset + i->second, color, alpha);
		alpha -= alpha_trail;
	}
}

void Particle::tick( void )
{
	if (ticks_to_live > 0)
		--ticks_to_live;
	
	x_vel += x_accel;
	y_vel += y_accel;
	
	x_vel = min(max(-term_vel, x_vel), term_vel);
	y_vel = min(max(-term_vel, y_vel), term_vel);
	
	x += x_vel;
	y += y_vel;
	
	alpha = min(max(Fixed(SDL_ALPHA_TRANSPARENT), alpha + alpha_per_tick), Fixed(SDL_ALPHA_OPAQUE));
	
	trail.push_back(coord(x, y));
	if (trail.size() > trail_max)
		trail.pop_front();
}

void Particle::tick_all( std::list<Particle> &particles )
{
	for (std::list<Particle>::iterator i = particles.begin(); i != particles.end(); )
	{
		i->tick();
		
		if (i->ticks_to_live == 0)
			i = particles.erase(i);
		else
			++i;
	}
}

ExplosionParticle::ExplosionParticle( Fixed x, Fixed y, const SDL_Color &color )
: Particle(x, y, rand() % 50 + 20, color)
{
	x_vel = make_frac<Fixed>(rand() % (1024 * 2) - 1024, 1024);  // [-1..1]
	y_vel = make_frac<Fixed>(rand() % (1024 * 2) - 1024, 1024);  // [-1..1]
	
	alpha = SDL_ALPHA_OPAQUE + static_cast<int>(ticks_to_live) - 50 - 20;
	alpha_per_tick = -alpha / static_cast<int>(ticks_to_live);
}

DustParticle::DustParticle( Fixed x, Fixed y, const SDL_Color &color )
: Particle(x, y, rand() % 60 + 20, color)
{
	x_vel = make_frac<Fixed>(rand() % (1024 * 2) - 1024, 1024) / 5;  // [-1/5..1/5]
	y_vel = make_frac<Fixed>(rand() % 1024, 1024) / 5;  // [0..1/5]
	
	alpha = SDL_ALPHA_OPAQUE + static_cast<int>(ticks_to_live) - 60 - 20;
	alpha_per_tick = -alpha / static_cast<int>(ticks_to_live);
}

StarDustParticle::StarDustParticle( Fixed x, Fixed y, const SDL_Color &color )
: Particle(x, y, rand() % 60 + 20, color)
{
	x_vel = make_frac<Fixed>(rand() % (1024 * 2) - 1024, 1024) / 5;  // [-1/5..1/5]
	y_vel = make_frac<Fixed>(rand() % 1024, 1024) / 5;  // [0..1/5]
	
	y_accel = -y_accel;
	
	alpha = SDL_ALPHA_OPAQUE + static_cast<int>(ticks_to_live) - 60 - 20;
	alpha_per_tick = -alpha / static_cast<int>(ticks_to_live);
}

SparkParticle::SparkParticle( Fixed x, Fixed y, Fixed x_vel, Fixed y_vel, const SDL_Color &color )
: Particle(x, y, rand() % 20 + 20, color)
{
	this->x_vel = make_frac<Fixed>(rand() % (1024 * 2) - 1024, 1024) / 5 + x_vel;  // [-1/5..1/5] + ...
	this->y_vel = make_frac<Fixed>(rand() % (1024 * 2) - 1024, 1024) / 5 + y_vel;  // [-1/5..1/5] + ...
	
	alpha = SDL_ALPHA_OPAQUE + static_cast<int>(ticks_to_live) - 20 - 20;
	alpha_per_tick = -alpha / static_cast<int>(ticks_to_live);
}
