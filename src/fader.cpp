#include "loop.hpp"

using std::max;
using std::min;

Fader::Fader( uint ms )
: done(true),
  alpha(SDL_ALPHA_TRANSPARENT), target(alpha), step(255 / ms_to_updates(ms == 0 ? 500 : ms))
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
	done = (alpha == target);
	if (!done)
		alpha = min(max(SDL_ALPHA_TRANSPARENT, alpha + step), SDL_ALPHA_OPAQUE);
}
