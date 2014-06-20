#ifndef SOUND_H
#define SOUND_H

#include "s3e.h"

class Sound
{
public:
	Sound(const char* apFileName);
	~Sound();

	void Play();

private:
	int16* mpSoundData;
	uint32 mSamples;
};

#endif
