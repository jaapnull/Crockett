#pragma once
#include <WCommon/Window.h>
#include <WCommon/DIB.h>
#include <WCommon/Canvas.h>
#include <WCommon/ColorPen.h>
#include <WCommon/Font.h>
#include <WCommon/Mouse.h>
#include <WCommon/Keyboard.h>

#include "Palette.h"

class PaletteWindow : public Window, public IPaintHandler, IMouseHandler, IKeyHandler
{
public:
	PaletteWindow();

	Palette* GetPalette() const { return mPalette; }
	void SetPalette(Palette* inPalette);
	IRect GetDrawRectFromIndex(const ivec2& inIndex);
	ivec2 GetScale(const ivec2& inOutputSize);
	ivec2 GetIndexFromPosition(const ivec2& inPosition);


	// component events
	virtual void OnSizing(EnumMask<ERectEdge> inEdge, IRect& ioRect) override;
	virtual void OnSized(const ivec2& inNewSize) override;
	virtual void OnMouseLeave() override;
	virtual void OnMouseMove(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override;
	virtual void OnMouseLeftDown(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override;
	virtual void OnUpdate(DIB& inDib, const IRect& inRegion) override;

private:
	uint						mDrawScale = 1;
	Palette*					mPalette;

	ivec2						mSelectedIndex;
	ivec2						mMouseOverIndex;

	// components
	KeyboardHandler				mKeyboard;
	MouseHandler				mMouseHandler;
	Canvas						mCanvas;
};