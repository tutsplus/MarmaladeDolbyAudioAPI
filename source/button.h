#ifndef BUTTON_H
#define BUTTON_H

#include "IwGx.h"

//-----------------------------------------------------------------------------
class Button
{
public:
	Button(CIwTexture* apTexture, int32 aX, int32 aY, int32 aWidth, int32 aHeight, int32 aU, int32 aV, int32 aUWidth, int32 aVWidth, bool aEnabled);
	~Button();

	void Render();
	void Update(uint32 aTouchState, int32 aX, int32 aY);
	bool IsReleased();

private:
	CIwMaterial* mpMaterial;
	CIwSVec2 mTopLeft;
	CIwSVec2 mSize;
	CIwFVec2 mUV;
	CIwFVec2 mUVSize;
	bool mEnabled;
	bool mPressed;
	bool mDown;
	bool mReleased;
};

#endif
