/*  audio/channel.cpp
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
#include "audio/channel.hpp"

using std::max;
using std::min;

bool reverse_stereo = false;

Channel::Channel( Fixed volume, Fixed pan )
: volume(min(max(Fixed(0), volume), Fixed(1))), pan(min(max(Fixed(0), pan), Fixed(1)))
{
	// good to go
}

Channel::Channel( const Channel &that )
{
	copy(that);
}

void Channel::copy( const Channel &that )
{
	volume = that.volume;
	pan = that.pan;
}
