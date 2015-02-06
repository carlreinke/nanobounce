/*  audio/stream.cpp
 *  
 *  Copyright 2010 Carl Reinke
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
#include "audio/audio.hpp"
#include "audio/stream.hpp"

using std::max;
using std::min;

Stream::Stream( const std::string &path )
: Channel(),
  buffer(), size(0), start_position(0), end_position(0),
  end_of_file(true),
  vorbis_file(NULL)
{
	if (audio_disabled)
		return;
	
	vorbis_file = new OggVorbis_File;
	
#if defined(_WIN32)
	if (ov_fopen(const_cast<char *>(path.c_str()), vorbis_file) != 0)
#else
	FILE *f = fopen(path.c_str(), "rb");
	if (!f || ov_open(f, vorbis_file, NULL, 0) != 0)
#endif
	{
		std::cerr << "failed to load audio stream from '" << path << "'" << std::endl;
		
		delete vorbis_file;
		vorbis_file = NULL;
		return;
	}
	
	// prepare for audio conversion
	int channels = ov_info(vorbis_file, -1)->channels;
	long freq = ov_info(vorbis_file, -1)->rate;
	
	if (SDL_BuildAudioCVT(&cvt, AUDIO_S16SYS, channels,      freq,
	                            spec.format,  spec.channels, spec.freq) == -1)
	{
		std::cerr << "failed to convert audio stream from '" << path << "'" << std::endl;
		return;
	}
	
	// make buffer sufficiently large
	size = max(static_cast<uint>(2 * sizeof(Sint16) * spec.channels * spec.samples), static_cast<uint>(4096 * cvt.len_mult));
	buffer = boost::shared_array<Uint8>(new Uint8[size]);
	
	end_of_file = false;
}

Stream::~Stream( void )
{
	destroy();
}


bool Stream::empty( void ) const
{
	return end_of_file;
}

void Stream::rewind( void )
{
	if (ov_raw_seek(vorbis_file, 0) == 0)
		end_of_file = false;
}


void Stream::destroy( void )
{
	if (vorbis_file != NULL)
	{
		ov_clear(vorbis_file);
		delete vorbis_file;
		vorbis_file = NULL;
	}
}

Uint8 * Stream::get_buffer( uint &len )
{
	assert(len <= size);
	
	// shift data to front of buffer if necessary
	if (start_position + len > size)
	{
		memmove(&buffer[0], &buffer[start_position], end_position - start_position);
		end_position -= start_position;
		start_position = 0;
	}
	
	while (end_position - start_position < len && !end_of_file)
	{
		// fill empty section at end of buffer with raw audio
#if defined(USE_TREMOR)
		int read = ov_read(vorbis_file, reinterpret_cast<char *>(&buffer[end_position]), (size - end_position) / cvt.len_mult, &bitstream);
#else
		int read = ov_read(vorbis_file, reinterpret_cast<char *>(&buffer[end_position]), (size - end_position) / cvt.len_mult, 0, 2, 1, &bitstream);
#endif
		switch (read)
		{
		case OV_HOLE:
			break;
			
		case OV_EBADLINK:
		case OV_EINVAL:
		case 0:
			end_of_file = true;
			break;
			
		default:
			// convert the section of raw audio
			cvt.buf = &buffer[end_position];
			cvt.len = read;
			
			SDL_ConvertAudio(&cvt);
			
			end_position += cvt.len_cvt;
			break;
		}
	}
	
	len = min(len, end_position - start_position);
	
	return &buffer[start_position];
}

void Stream::flush( uint len )
{
	start_position += len;
}
