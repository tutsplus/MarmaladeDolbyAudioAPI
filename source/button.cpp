#include "button.h"
#include "s3e.h"

Button::Button(CIwTexture* apTexture, int32 aX, int32 aY, int32 aWidth, int32 aHeight, int32 aU, int32 aV, int32 aUWidth, int32 aVHeight, bool aEnabled)
{
    mpMaterial = new CIwMaterial();
    mpMaterial->SetTexture(apTexture);
    
    float lTextureWidth = (float) apTexture->GetWidth();
    float lTextureHeight = (float) apTexture->GetHeight();
    
    mTopLeft.x = aX;
    mTopLeft.y = aY;
    mSize.x = aWidth;
    mSize.y = aHeight;
    mUV.x = (float) aU / lTextureWidth;
    mUV.y = (float) aV / lTextureHeight;
    mUVSize.x = (float) aUWidth / lTextureWidth;
    mUVSize.y = (float) aVHeight / lTextureHeight;
    
    mEnabled = aEnabled;
	mDown = false;
	mPressed = false;
	mReleased = false;
}

Button::~Button()
{
    delete mpMaterial;
}

void Button::Render()
{
	if (!mEnabled)
		mpMaterial->SetColAmbient(96, 96, 96, 255);
	else if (mDown)
		mpMaterial->SetColAmbient(192, 192, 192, 255);
	else
		mpMaterial->SetColAmbient(255, 255, 255, 255);

    IwGxSetMaterial(mpMaterial);
    IwGxDrawRectScreenSpace(&mTopLeft, &mSize, &mUV, &mUVSize);
}

void Button::Update(uint32 aTouchState, int32 aX, int32 aY)
{
	if (!mEnabled)
		return;

	// Check if the touch position is within bounds of this button
	aX -= mTopLeft.x;
	aY -= mTopLeft.y;

	bool lInBounds = (aX >= 0) && (aX < mSize.x) && (aY >= 0) && (aY < mSize.y);

	// Clear the released flag
	mReleased = false;

	// Check touch screen state
	if (aTouchState & S3E_POINTER_STATE_PRESSED)
	{
		// User has just touched the screen
		if (lInBounds)
		{
			mPressed = true;
			mDown = true;
		}
	}
	else if (aTouchState & S3E_POINTER_STATE_DOWN)
	{
		// If button has been pressed, check if user is still touching it
		if (mPressed)
		{
			mDown = lInBounds;
		}
	}
	else if (aTouchState & S3E_POINTER_STATE_RELEASED)
	{
		// If user has released screen over a pressed button, we set the
		// release flag to true
		if (mPressed && mDown)
		{
			mReleased = true;
		}

		// Button is no longer pressed or down!
		mDown = false;
		mPressed = false;
	}
}

bool Button::IsReleased()
{
	return mReleased;
}
