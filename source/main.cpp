#include "IwGx.h"
#include "s3e.h"
#include "button.h"
#include "sound.h"

#include "s3eDolbyAudio.h"

CIwTexture* gpTexture = NULL;

enum ButtonIDs
{
	BUTTON_AUDIO_LABEL,
	BUTTON_AUDIO_OFF,
	BUTTON_AUDIO_MUSIC,
	BUTTON_AUDIO_SFX,
	BUTTON_AUDIO_SPEECH,
	BUTTON_FILTER_LABEL,
	BUTTON_FILTER_OFF,
	BUTTON_FILTER_MOVIE,
	BUTTON_FILTER_MUSIC,
	BUTTON_FILTER_GAME,
	BUTTON_FILTER_VOICE,
	BUTTON_COUNT
};

Button* gButton[BUTTON_COUNT];

Sound* gpGunBattleSound;
Sound* gpFemaleCountingSound;

bool gDolbySupported;
bool gDolbyInitialised;

int32 AppSuspended(void* apSystemData, void* apUserData)
{
    if (gDolbyInitialised)
        s3eDolbyAudioSuspendSession();
    return 0;
}

int32 AppResumed(void* apSystemData, void* apUserData)
{
    if (gDolbyInitialised)
        s3eDolbyAudioRestartSession();
    return 0;
}

void Initialise()
{
	// Initialise Marmalade modules
	IwGxInit();

	// Create a new CIwTexture and use it to load the GUI image file
	gpTexture = new CIwTexture;
	gpTexture->LoadFromFile("ui.png");
	gpTexture->Upload();

	// Check for Dolby Digital Plus support
	gDolbySupported = false;
	gDolbyInitialised = false;
	if (s3eDolbyAudioAvailable() == S3E_TRUE)
	{
		if (s3eDolbyAudioSupported() == S3E_TRUE)
		{
			gDolbySupported = true;
		}
		s3eDeviceYield(0);
	}

	// Initialise Pause/Resume callbacks
	s3eDeviceRegister(S3E_DEVICE_PAUSE, AppSuspended, NULL);
	s3eDeviceRegister(S3E_DEVICE_BACKGROUND, AppSuspended, NULL);
	s3eDeviceRegister(S3E_DEVICE_UNPAUSE, AppResumed, NULL);
	s3eDeviceRegister(S3E_DEVICE_FOREGROUND, AppResumed, NULL);

	// Create our interface buttons
	int32 lSize = IwGxGetScreenWidth() / 5;
	int32 lGap = (int32) ((float) lSize * 0.1f);
	lSize = (int32) ((float) lSize * 0.9f);

	int32 lRowSize = IwGxGetScreenHeight() / 4;
	int32 lTopRowX = (IwGxGetScreenWidth() - (4 * lSize) - (3 * lGap)) / 2;
	int32 lTopRowY = lRowSize - (lSize / 2);
	int32 lBottomRowX = (IwGxGetScreenWidth() - (5 * lSize) - (4 * lGap)) / 2;
	int32 lBottomRowY = (3 * lRowSize) - (lSize / 2);
	int32 lLabelWidth = (240 * lSize) / 160;
	int32 lLabelHeight = (42 * lSize) / 160;
	int32 lLabelX = (IwGxGetScreenWidth() - lLabelWidth) / 2;

	gButton[BUTTON_AUDIO_LABEL] = new Button(gpTexture, lLabelX, lTopRowY - lLabelHeight - 10, lLabelWidth, lLabelHeight, 4, 408, 240, 42, false);
	gButton[BUTTON_AUDIO_OFF] = new Button(gpTexture, lTopRowX, lTopRowY, lSize, lSize, 347, 3, 160, 160, true);
	gButton[BUTTON_AUDIO_MUSIC] = new Button(gpTexture, lTopRowX + (lSize + lGap), lTopRowY, lSize, lSize, 175, 3, 160, 160, true);
	gButton[BUTTON_AUDIO_SFX] = new Button(gpTexture, lTopRowX + (2 * (lSize + lGap)), lTopRowY, lSize, lSize, 2, 173, 160, 160, true);
	gButton[BUTTON_AUDIO_SPEECH] = new Button(gpTexture, lTopRowX + (3 * (lSize + lGap)), lTopRowY, lSize, lSize, 174, 173, 160, 160, true);
	gButton[BUTTON_FILTER_LABEL] = new Button(gpTexture, lLabelX, lBottomRowY - lLabelHeight - 10, lLabelWidth, lLabelHeight, 2, 353, 240, 42, false);
	gButton[BUTTON_FILTER_OFF] = new Button(gpTexture, lBottomRowX, lBottomRowY, lSize, lSize, 347, 3, 160, 160, gDolbySupported);
	gButton[BUTTON_FILTER_MOVIE] = new Button(gpTexture, lBottomRowX + (lSize + lGap), lBottomRowY, lSize, lSize, 2, 3, 160, 160, gDolbySupported);
	gButton[BUTTON_FILTER_MUSIC] = new Button(gpTexture, lBottomRowX + (2 * (lSize + lGap)), lBottomRowY, lSize, lSize, 175, 3, 160, 160, gDolbySupported);
	gButton[BUTTON_FILTER_GAME] = new Button(gpTexture, lBottomRowX + (3 * (lSize + lGap)), lBottomRowY, lSize, lSize, 2, 173, 160, 160, gDolbySupported);
	gButton[BUTTON_FILTER_VOICE] = new Button(gpTexture, lBottomRowX + (4 * (lSize + lGap)), lBottomRowY, lSize, lSize, 174, 173, 160, 160, gDolbySupported);

	// Load sound effects into memory
	gpGunBattleSound = new Sound("gun-battle.raw");
	gpFemaleCountingSound = new Sound("female-counting.raw");

	// Configure default sample rate for s3eSound
	s3eSoundSetInt(S3E_SOUND_DEFAULT_FREQ, 44100);
}

void Terminate()
{
	// Destroy sound effects
	delete gpGunBattleSound;
	delete gpFemaleCountingSound;

	// Destroy Button instances
	for (uint32 i = 0; i < BUTTON_COUNT; i++)
	{
		delete gButton[i];
	}

	// Destroy texture instance
	delete gpTexture;

	// Disable Pause/Resume callbacks
	s3eDeviceUnRegister(S3E_DEVICE_PAUSE, AppSuspended);
	s3eDeviceUnRegister(S3E_DEVICE_BACKGROUND, AppSuspended);
	s3eDeviceUnRegister(S3E_DEVICE_UNPAUSE, AppResumed);
	s3eDeviceUnRegister(S3E_DEVICE_FOREGROUND, AppResumed);

	// Terminate Marmalade modules
	IwGxTerminate();
}

void Update()
{
	// Allow device OS time to do its processing
	s3eDeviceYield(0);

	// Update pointer (actually touch screen!) inputs
	s3ePointerUpdate();

	// Read current touch screen inputs and use them to update Button states
	uint32 lTouchState = s3ePointerGetState(S3E_POINTER_BUTTON_SELECT);
	int32 x = s3ePointerGetX();
	int32 y = s3ePointerGetY();
	for (uint32 i = 0; i < BUTTON_COUNT; i++)
	{
		gButton[i]->Update(lTouchState, x, y);
	}

	// Check for button presses
	if (gButton[BUTTON_AUDIO_MUSIC]->IsReleased())
	{
		s3eAudioPlay("black-hole.mp3");
	}
	else if (gButton[BUTTON_AUDIO_SFX]->IsReleased())
	{
		if (gpGunBattleSound)
			gpGunBattleSound->Play();
	}
	else if (gButton[BUTTON_AUDIO_SPEECH]->IsReleased())
	{
		if (gpFemaleCountingSound)
			gpFemaleCountingSound->Play();
	}
	else if (gButton[BUTTON_AUDIO_OFF]->IsReleased())
	{
		s3eAudioStop();
		s3eSoundStopAllChannels();
	}

	if (gButton[BUTTON_FILTER_OFF]->IsReleased())
	{
		if (gDolbyInitialised)
		{
			s3eDolbyAudioSetEnabled(S3E_FALSE);
		}
	}
	else if (gButton[BUTTON_FILTER_MUSIC]->IsReleased())
	{
		if (gDolbyInitialised)
		{
			s3eDolbyAudioSetEnabled(S3E_TRUE);
			s3eDolbyAudioSetProfile(MUSIC);
		}
	}
	else if (gButton[BUTTON_FILTER_MOVIE]->IsReleased())
	{
		if (gDolbyInitialised)
		{
			s3eDolbyAudioSetEnabled(S3E_TRUE);
			s3eDolbyAudioSetProfile(MOVIE);
		}
	}
	else if (gButton[BUTTON_FILTER_GAME]->IsReleased())
	{
		if (gDolbyInitialised)
		{
			s3eDolbyAudioSetEnabled(S3E_TRUE);
			s3eDolbyAudioSetProfile(GAME);
		}
	}
	else if (gButton[BUTTON_FILTER_VOICE]->IsReleased())
	{
		if (gDolbyInitialised)
		{
			s3eDolbyAudioSetEnabled(S3E_TRUE);
			s3eDolbyAudioSetProfile(VOICE);
		}
	}
}

void Render()
{
	// Clear the screen to a pale blue
	IwGxSetColClear(128, 224, 255, 0);
	IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);

	// Render the UI
	for (uint32 i = 0; i < BUTTON_COUNT; i++)
	{
		gButton[i]->Render();
	}

	// Finish rendering and display on screen
	IwGxFlush();
	IwGxSwapBuffers();
}

int main()
{
	Initialise();

	while (!s3eDeviceCheckQuitRequest())
	{
		Update();
		Render();
	}

	Terminate();
	return 0;
}
