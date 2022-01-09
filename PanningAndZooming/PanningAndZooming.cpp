
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <bitset>
#include <sstream>
#include <functional>

#include <LibN64.h>

using namespace LibN64;

double timer;

class PanAndZoom : public Frame  {
public:
  	PanAndZoom(resolution_t res, bitdepth_t dep, antialias_t aa, UIType ui) : Frame(res, dep, aa, ui) {}

private:

	float fOffsetX = 0.0f;
	float fOffsetY = 0.0f;
	float fScaleX = 1.0f;
	float fScaleY = 1.0f;

	float fStartPanX = 0.0f;
	float fStartPanY = 0.0f;

	float fSelectedCellX = 0.0f;
	float fSelectedCellY = 0.0f;

	float fMouseX = ScreenWidth()/2, fMouseY = ScreenHeight()/2;
	bool  bSelectedCell = false;
	bool bPanning = false;
	void WorldToScreen(float fWorldX, float fWorldY, int &nScreenX, int &nScreenY)
	{
		nScreenX = (int)((fWorldX - fOffsetX) * fScaleX);
		nScreenY = (int)((fWorldY - fOffsetY) * fScaleY);
	}

	void ScreenToWorld(int nScreenX, int nScreenY, float &fWorldX, float &fWorldY)
	{
		fWorldX = ((float)nScreenX / fScaleX) + fOffsetX;
		fWorldY = ((float)nScreenY / fScaleY) + fOffsetY;
	}
protected:

	void OnCreate() override 
	{
		SetDLInLoop();
		fOffsetX = -ScreenWidth() / 2;
		fOffsetY = -ScreenHeight() / 2;
		
	}
	
	bool bZoomingIn = false, bZoomingOut = false;
	void FrameUpdate() override 
	{ 
		if(fMouseX < 0) 
			fMouseX = 0;
		if(fMouseX > ScreenWidth())
			fMouseX = ScreenWidth();
		if(fMouseY < 0) 
			fMouseY = 0;
		if(fMouseY > ScreenHeight()) 
			fMouseY = ScreenHeight();
		if(bPanning) {
			fOffsetX -= (fMouseX - fStartPanX) / fScaleX;
			fOffsetY -= (fMouseY - fStartPanY) / fScaleY;

			fStartPanX = fMouseX;
			fStartPanY = fMouseY;

			bPanning = false;
		}
		float fMouseWorldX_BeforeZoom, fMouseWorldY_BeforeZoom;
		ScreenToWorld(fMouseX, fMouseY, fMouseWorldX_BeforeZoom, fMouseWorldY_BeforeZoom);
		
		if(bZoomingIn) {
			fScaleX *= 1.001f;
			fScaleY *= 1.001f;
			bZoomingIn = false;
		} 
		if(bZoomingOut) {
			fScaleX *= 0.999f;
			fScaleY *= 0.999f;
			bZoomingOut = false;
		}

		float fMouseWorldX_AfterZoom, fMouseWorldY_AfterZoom;
		ScreenToWorld(fMouseX, fMouseY, fMouseWorldX_AfterZoom, fMouseWorldY_AfterZoom);		
		fOffsetX += (fMouseWorldX_BeforeZoom - fMouseWorldX_AfterZoom);
		fOffsetY += (fMouseWorldY_BeforeZoom - fMouseWorldY_AfterZoom);

		ClearScreenRDP();

		// Clip
		float fWorldLeft, fWorldTop, fWorldRight, fWorldBottom;
		ScreenToWorld(0, 0, fWorldLeft, fWorldTop);
		ScreenToWorld(ScreenWidth(), ScreenHeight(), fWorldRight, fWorldBottom);
		
		auto function = [](float x)
		{
			return sinf(x);
		};

		
		DrawPixel({fMouseX, fMouseY}, RED);
		// Draw Main Axes a 10x10 Unit Grid
		// Draw 10 horizontal lines
		int nLinesDrawn = 0;
		for (float y = 0.0f; y <= 10.0f; y++)
		{
			if (y >= fWorldTop && y <= fWorldBottom)
			{
				float sx = 0.0f, sy = y;
				float ex = 10.0f, ey = y;

				int pixel_sx, pixel_sy, pixel_ex, pixel_ey;

				WorldToScreen(sx, sy, pixel_sx, pixel_sy);
				WorldToScreen(ex, ey, pixel_ex, pixel_ey);

				DrawLine({pixel_sx, pixel_sy},{ pixel_ex, pixel_ey},LibColor::WHITE);
				nLinesDrawn++;
			}
		}

		// Draw 10 vertical lines
		for (float x = 0.0f; x <= 10.0f; x++)
		{
			if (x >= fWorldLeft && x <= fWorldRight)
			{
				float sx = x, sy = 0.0f;
				float ex = x, ey = 10.0f;

				int pixel_sx, pixel_sy, pixel_ex, pixel_ey;

				WorldToScreen(sx, sy, pixel_sx, pixel_sy);
				WorldToScreen(ex, ey, pixel_ex, pixel_ey);

				DrawLine({pixel_sx, pixel_sy}, {pixel_ex, pixel_ey}, LibColor::WHITE);
				nLinesDrawn++;
			}
		}

		if(bSelectedCell)
		{
			fSelectedCellX = (int)fMouseWorldX_AfterZoom;
			fSelectedCellY = (int)fMouseWorldY_AfterZoom;
			bSelectedCell = false;
		}

		int cx, cy, cr;
		WorldToScreen(fSelectedCellX + 0.5f, fSelectedCellY + 0.5f, cx, cy);
		cr = 0.3f * fScaleX;
		DrawCircle({cx,cy},1*cr, 0.1, RED, false);

		DrawText({5,5}, "Lines Drawn: " + std::to_string(nLinesDrawn));
		DrawText({5,15},"X: " + std::to_string(fMouseX) + " Y: " + std::to_string(fMouseY));
	}

	void KeyAPressed() {
		fStartPanX = fMouseX;
		fStartPanY = fMouseY;
		bPanning = true;
	}

	void KeyDUPressed() {
		bZoomingIn = true;
	}
	void KeyDDPressed() {
		bZoomingOut = true;
	}

	void KeyBPressed() {
		bSelectedCell = true;
	}

	void KeyJoyYPressed(int dir) {
		switch(dir) {
			case JoyUp:  fMouseY--; break;
			case JoyDown: fMouseY++; break;
			default: break;
		}
	}

	void KeyJoyXPressed(int dir) {
		switch(dir) {
			case JoyLeft: fMouseX--; break;
			case JoyRight: fMouseX++; break;
			default: break;
		}
	}


};

int main(void) {
	PanAndZoom n64g(RESOLUTION_320x240, DEPTH_32_BPP, ANTIALIAS_OFF, Frame::UIType::GUI);
	n64g.Begin();
	
	return 0;
}
