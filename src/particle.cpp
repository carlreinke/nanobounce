#include "ball.hpp"
#include "particle.hpp"

using namespace std;

Particle::Particle( Fixed x, Fixed y, uint ticks_to_live, const SDL_Color &color )
: x(x), y(y),
  x_vel(0), y_vel(0),
  x_accel(0), y_accel(Ball::y_accel),
  x_term_vel(Ball::x_term_vel), y_term_vel(Ball::y_term_vel),
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
	
	for (deque<coord>::const_reverse_iterator i = trail.rbegin(); i != trail.rend(); ++i)
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

FireworkParticle::FireworkParticle( Fixed x, Fixed y, const SDL_Color &color )
: Particle(x, y, rand() % 20 + 30, color)
{
	const Fixed radius = make_frac<Fixed>(rand() % (1024 * 2) - 1024, 1024),  // [-1..1]
	            angle = make_frac<Fixed>(rand() % 31415, 10000);  // [0..pi]
	
	x_vel = cosf(angle) * radius,
	y_vel = sinf(angle) * radius;
	
	y_accel /= 2;
	
	x_term_vel = y_term_vel = 100;  // large enough to be irrelevant
	
	alpha = SDL_ALPHA_OPAQUE + static_cast<int>(ticks_to_live) - 20 - 30;
	alpha_per_tick = -alpha / static_cast<int>(ticks_to_live);
}
