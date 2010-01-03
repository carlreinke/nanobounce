#include "ball.hpp"
#include "particle.hpp"
#include "sdl_ext.hpp"

using namespace std;

Particle::Particle( Fixed x, Fixed y, uint ticks_to_live )
: x(x), y(y),
  x_vel(0), y_vel(0),
  x_accel(0), y_accel(Ball::y_accel),
  x_term_vel(Ball::x_term_vel), y_term_vel(Ball::y_term_vel),
  ticks_to_live(ticks_to_live)
{
	/* nothing to do */
}

void Particle::draw( SDL_Surface *surface, int x_offset, int y_offset, Uint8 alpha ) const
{
	sprite->blit(surface, x_offset + x, y_offset + y, alpha);
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
	
	if (sprite != last_sprite && ticks_to_live % next_sprite == 0)
		++sprite;
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

vector<Sprite> ExplosionParticle::sprites;

ExplosionParticle::ExplosionParticle( Fixed x, Fixed y )
: Particle(x, y, rand() % 25 + 25)
{
	if (sprites.empty())
	{
		for (int i = 3; i > 0; --i)
		{
			uint c = 255 - 128 * i / 4;
			sprites.push_back(Sprite(i, i, SDL_Color_RGBA(c, c / 4, c / 4)));
		}
	}
	
	x_vel = Fixed(rand() % (1024 * 2) - 1024) / 1024;
	y_vel = Fixed(rand() % (1024 * 2) - 1024) / 1024;
	
	y_accel /= 2;
	
	sprite = sprites.begin();
	last_sprite = sprites.end() - 1;
	
	next_sprite = ticks_to_live / 3;
}

vector<Sprite> DustParticle::sprites;

DustParticle::DustParticle( Fixed x, Fixed y )
: Particle(x, y, rand() % 50 + 50)
{
	if (sprites.empty())
	{
		for (int i = 3; i > 0; --i)
		{
			uint c = 128;
			sprites.push_back(Sprite(i, i, SDL_Color_RGBA(c, c, c)));
		}
	}
	
	x_vel = (Fixed(rand() % (1024 * 2) - 1024) / 1024) / 5;
	y_vel = (Fixed(rand() % 1024) / 1024) / 5;
	
	sprite = sprites.begin();
	last_sprite = sprites.end() - 1;
	
	next_sprite = ticks_to_live / 3;
}
