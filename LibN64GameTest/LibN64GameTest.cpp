#include <stdio.h>
#include <stdlib.h>
#include <LibN64.h>
#include <iostream>
#include <string.h>
#include <memory>
#include "LibN64GameTest.h"

extern void *__safe_buffer[];

#define DARK_GREY graphics_make_color(136, 138, 137, 255)
#define VERY_DARK_GREY graphics_make_color(77, 79, 77, 255)

/*test of pseudo-3D planes on LibDragon.*/

std::string map = "################################" 
"#...............#..............."
"#.......#########.......########"
"#..............##..............#"
"#......##......##......##......#"
"#......##..............##......#"
"#..............##..............#"
"###............####............#"
"##.............###.............#"
"#............####............###"
"#.....&........................#"
"#..............##..............#"
"#.....&........##..............#"
"#...........#####...........####"
"#..............................#"
"###..####................#######"
"####.####......................."
"#..............................."
"#.......................##..####"
"#..............##..............#"
"#......##......##.......#......#"
"#......##......##......##......#"
"#..............##..............#"
"###............####............#"
"##.............###.............#"
"#............####............###"
"#..............................#"
"#..............................#"
"#..............##..............#"
"#...........##..............####"
"#..............##..............#"
"################################";

using namespace LibN64;

class Mode7Test : public Frame {
public:
	Mode7Test(resolution_t res, bitdepth_t dep, antialias_t aa, UIType ui) : Frame(res, dep, aa, ui) {}

	sprite_t *wall  = nullptr;
	sprite_t *sky   = nullptr; 
	sprite_t *grass = nullptr;
	sprite_t *wiz   = nullptr;

	float fWorldX;
	float fWorldY;
	float fWorldA;
	float fNear;
	float fFar;
	float fFoVHalf;

	int nMapSize;

	int nMapWidth;			
	int nMapHeight;

	float fPlayerX;		
	float fPlayerY;
	float fPlayerA; 		
	float fFOV;	
	float fDepth;		
	float fSpeed;		

	float *fDepthBuffer = new float[ScreenWidth()];

	bool objPressed;
	bool drawwall = false;
	LibPos dw_pos;
	float dw_dist;

	void DrawMode() {
		for (uint32_t x = 0; x < ScreenWidth(); x++)
		{
			// For each column, calculate the projected ray angle into world space
			float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)ScreenWidth()) * fFOV;

			// Find distance to wall
			float fStepSize = 0.1f;	 
			float fDistanceToWall = 0.0f; //                                    
			float fDistanceToObj = 0.0f;

			bool bHitWall = false;		
			
			bool bHitObj = false;
		
			float fEyeX = sinf(fRayAngle); 
			float fEyeY = cosf(fRayAngle);

			float fSampleX = 0.0f;
		
			while (!bHitWall && fDistanceToWall < fDepth)
			{
				fDistanceToWall += fStepSize;
				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

				// Test if ray is out of bounds
				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
				{
					bHitWall = true;			// Just set distance to maximum depth
					fDistanceToWall = fDepth;
				}
				else
				{
					// Ray is inbounds so test to see if the ray cell is a wall block
					if (map[nTestX * nMapWidth + nTestY] == '#')
					{
						// Ray has hit wall
						bHitWall = true;

					//	DrawPixel({nTestX, nTestY}, RED);

						float fBlockMidX = (float)nTestX + 0.5f;
						float fBlockMidY = (float)nTestY + 0.5f;

						float fTestPointX = fPlayerX + fEyeX * fDistanceToWall;
						float fTestPointY = fPlayerY + fEyeY * fDistanceToWall;

						float fTestAngle = atan2f((fTestPointY - fBlockMidY), (fTestPointX - fBlockMidX));

						if (fTestAngle >= -3.14159f * 0.25f && fTestAngle < 3.14159f * 0.25f)
							fSampleX = fTestPointY - (float)nTestY;
						if (fTestAngle >= 3.14159f * 0.25f && fTestAngle < 3.14159f * 0.75f)
							fSampleX = fTestPointX - (float)nTestX;
						if (fTestAngle < -3.14159f * 0.25f && fTestAngle >= -3.14159f * 0.75f)
							fSampleX = fTestPointX - (float)nTestX;
						if (fTestAngle >= 3.14159f * 0.75f || fTestAngle < -3.14159f * 0.75f)
							fSampleX = fTestPointY - (float)nTestY;

					}
				}
			}

			while (!bHitObj && fDistanceToObj < fDepth) {
				fDistanceToObj += fStepSize;
				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToObj);
				int nTestY = (int)(fPlayerY + fEyeY * fDistanceToObj);
				// Test if ray is out of bounds
				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
				{
					bHitObj = true;			// Just set distance to maximum depth
					fDistanceToObj = fDepth;
				}
				else
				{
					if (map[nTestX * nMapWidth + nTestY] == '&')
					{
						bHitObj = true;
					}
				}
			}

		
			// Calculate distance to ceiling and floor
			int nCeiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)fDistanceToWall);
			int nObjCeiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)fDistanceToObj);
			
			int nFloor = ScreenHeight() - nCeiling;
			int nObjFloor = ScreenHeight() - nObjCeiling;
			// Update Depth Buffer
			fDepthBuffer[x] = fDistanceToWall;
	
			for (int y = 0; y < (int)ScreenHeight(); y++)
			{
				if (y <= nCeiling) {
					float fSampleY = ((float)y - 0);
					int col = sky->data[int(fSampleY * 5.5 + fSampleX + fPlayerA)];
					DrawPixel({x,y}, col);
				}
				else if (y > nCeiling && y <= nFloor)
				{
					// Draw Wall
					float fSampleY = ((float)y - (float)nCeiling) / ((float)nFloor - (float)nCeiling);
					int col = wall->data[int(fSampleY * (wall->width*wall->height) + fSampleX)];
				
					if (fDistanceToWall > 0 && fDistanceToWall < 14)
					{
						DrawPixel({x,y}, col * fDistanceToWall * 0.0002);
					}
					else if (fDistanceToWall > 14) {
						DrawPixel({x,y}, 0X0);
					}
				}
				else // Floor
				{
					float b = 1.0f - (((float)y - ScreenHeight() / 2.0f) / ((float)ScreenHeight() / 2));
					float fSampleY = ((float)y) - fPlayerA / ScreenWidth();

					if(b < 0.25)   	    DrawPixel({x, y}, 0x909090FF);
					else if(b < 0.375)  DrawPixel({x, y}, 0x808080FF);
					else if(b < 0.5)    DrawPixel({x, y}, 0x707070FF);
					else if(b < 0.625)  DrawPixel({x, y}, 0x606060FF);
					else if(b < 0.75)   DrawPixel({x, y}, 0x505050FF);
					else if(b < 0.875)  DrawPixel({x, y}, 0x404040FF);
					else if(b < 0.9)    DrawPixel({x, y}, 0x303030FF);
			
				}
			
				if (y > nObjCeiling && y <= nObjFloor) {
					if (fDistanceToObj > 2 && fDistanceToObj < 12)
					{
						DrawPixel({x,y}, ORANGE *0x101010FF / fDistanceToObj);
					} 
			
				}
			}

		}

			//check obj picked up
			if(map[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '&') {
				Graphics::SetColor(ORANGE, SKY_BLUE);
				map[(int)fPlayerX * nMapWidth + fPlayerY] = '.';
			}

			Graphics::SetColor(RED, DARK_GREY);
			DrawTextFormat({0,(int)(ScreenHeight()/2)+40},
			"\nX: %0.2f (%d)\nY: %0.2f (%d)\nA: %0.2f FrameRate %0.2f", fPlayerX, (int)fPlayerX, fPlayerY, (int)fPlayerY, fPlayerA, GetFrameRate());

			
	}
	
	virtual void OnCreate() override
	{
		nMapWidth = 32;
		nMapHeight = 32;
		fPlayerX = 12.0f;			// Player Start Position
		fPlayerY = 5.09;// 5.09f;Graphics::DrawBox
		fFOV = 1.0;
		fDepth = 16.0f;			// Maximum rendering distance
		fSpeed = 1.0f;			// Walking Speed

		wall  = DFS::QuickRead<sprite_t*>("/wall.sprite");
		sky   = DFS::QuickRead<sprite_t*>("/sky.sprite");
		grass = DFS::QuickRead<sprite_t*>("/grass.sprite");
		wiz   = DFS::QuickRead<sprite_t*>("/wiz.sprite");

		SetKeyState(KeysPressed);
		DrawMode();

	
	}

protected:
	virtual void KeyJoyXPressed(int direction) override 
	{
		switch(direction) 
	   {
		   case JoyLeft: {
				fPlayerA -= (fSpeed * 0.2f);
				DrawMode();
				} break;
		   case JoyRight: {
				fPlayerA += (fSpeed * 0.2f);
				DrawMode();
			} break;
			default: break;
		}
	}

	virtual void KeyJoyYPressed(int direction) override
	{
	 	switch(direction) 
	   {
		   case Joystick::JoyUp : 
		   {
				fPlayerX += sinf(fPlayerA) * fSpeed;
				fPlayerY += cosf(fPlayerA) * fSpeed;
				if (map[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
				{
					fPlayerX -= sinf(fPlayerA) * fSpeed;
					fPlayerY -= cosf(fPlayerA) * fSpeed;
				}
				DrawMode();
	
		   } break;
		   case Joystick::JoyDown: 
		   {
			   	fPlayerX -= sinf(fPlayerA) * fSpeed;
				fPlayerY -= cosf(fPlayerA) * fSpeed;
				if (map[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
				{
					fPlayerX += sinf(fPlayerA) * fSpeed;
					fPlayerY += cosf(fPlayerA) * fSpeed;
				}
				DrawMode();
		   } break;
	   }

		
	}

	virtual void KeyZPressed() override {
		/*Alternate resolution*/
		res = ~res & 1;
		if (res) {
			SetScreen(RESOLUTION_320x240, DEPTH_32_BPP);
		}
		else {
			SetScreen(RESOLUTION_640x480, DEPTH_32_BPP);
		}
	}

private:
	int res;
};

int main(void) {
	Mode7Test n64g(RESOLUTION_256x240, DEPTH_32_BPP, ANTIALIAS_RESAMPLE, Frame::UIType::GUI);
	n64g.Begin();

	return 0;
}
