#include "audio.hpp"
#include "audio_stream.hpp"

using namespace std;

Stream::Stream( const string &path )
: Channel(),
  buffer(NULL), size(0),
  start_position(0), end_position(0), end_of_file(true)
{
#ifndef TARGET_WIN32
	FILE *f = fopen(path.c_str(), "rb");
	if (!f || ov_open(f, &vorbis_file, NULL, 0) != 0)
#else
	if (ov_fopen(const_cast<char *>(path.c_str()), &vorbis_file) != 0)
#endif
	{
		cerr << "failed to load audio stream from '" << path << "'" << endl;
		return;
	}
	
	// prepare for audio conversion
	int channels = ov_info(&vorbis_file, -1)->channels;
	long freq = ov_info(&vorbis_file, -1)->rate;
	
	if (SDL_BuildAudioCVT(&cvt, AUDIO_S16SYS, channels,      freq,
	                            spec.format,  spec.channels, spec.freq) == -1)
	{
		cerr << "failed to convert audio stream from '" << path << "'" << endl;
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
#ifndef TARGET_GP2X
		int read = ov_read(&vorbis_file, reinterpret_cast<char *>(&buffer[end_position]), (size - end_position) / cvt.len_mult, 0, 2, 1, &bitstream);
#else
		int read = ov_read(&vorbis_file, reinterpret_cast<char *>(&buffer[end_position]), (size - end_position) / cvt.len_mult, &bitstream);
#endif
		if (read > 0)
		{
			// convert the section of raw audio
			cvt.buf = &buffer[end_position];
			cvt.len = read;
			
			SDL_ConvertAudio(&cvt);
			
			end_position += cvt.len_cvt;
		}
		else
			end_of_file = true;
	}
	
	len = min(len, end_position - start_position);
	
	return &buffer[start_position];
}

void Stream::flush( uint len )
{
	start_position += len;
}

bool Stream::empty( void ) const
{
	return end_of_file;
}

void Stream::rewind( void )
{
	if (ov_raw_seek(&vorbis_file, 0) == 0)
		end_of_file = false;
}


void Stream::destroy( void )
{
	ov_clear(&vorbis_file);
}
