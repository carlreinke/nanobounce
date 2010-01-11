#include "loop.hpp"

using namespace std;

Fader::Fader( int step )
: done(true),
  alpha(SDL_ALPHA_TRANSPARENT), target(alpha), step(step == 0 ? 255 / ms_to_updates(300) : step)
{
	// good to go
}

void Fader::fade( Fader::Direction direction )
{
	done = false;
	target = static_cast<int>(direction);
	step = (target < alpha) ? -abs(step) : abs(step);
}

void Fader::update( void )
{
	if (!done)
	{
		alpha = min(max(SDL_ALPHA_TRANSPARENT, alpha + step), SDL_ALPHA_OPAQUE);
		done = (alpha == target);
	}
}
