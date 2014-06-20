#include "sound.h"

Sound::Sound(const char* apFileName)
{
    // Attempt to open the sound effect file
    s3eFile* f = s3eFileOpen(apFileName, "rb");
    if (f)
    {
        // Seek to end of file to find its length
        s3eFileSeek(f, 0, S3E_FILESEEK_END);
        
        // Number of samples is file size divided by the size of an int16
        mSamples = s3eFileTell(f) / sizeof(int16);
        s3eFileSeek(f, 0, S3E_FILESEEK_SET);
        
        // Allocate buffer for sound data
        mpSoundData = new int16[mSamples];
        
        // Read in sound data
        s3eFileRead(mpSoundData, sizeof(int16), mSamples, f);
        
        // Close the file
        s3eFileClose(f);
    }
    else
    {
    // File open failed, zero the member variables
        mpSoundData = NULL;
        mSamples = 0;
    }
}

Sound::~Sound()
{
    if (mpSoundData)
        delete[] mpSoundData;
}

void Sound::Play()
{
    if (mpSoundData)
    {
        int lChannel = s3eSoundGetFreeChannel();
        s3eSoundChannelPlay(lChannel, mpSoundData, mSamples, 0, 0);
    }
}

