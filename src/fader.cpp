/*  nanobounce/src/fader.cpp
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
