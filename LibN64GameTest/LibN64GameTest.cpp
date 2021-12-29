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

	float fWorldX;
	float fWorldY;
	float fWorldA;
	float fNear;
	float fFar;
	float fFoVHalf;

	int nMapSize;

	int nMapWidth;				// World Dimensions
	int nMapHeight;

	float fPlayerX;		// Player Start Position
	float fPlayerY;
	float fPlayerA; 			// Player Start Rotation
	float fFOV;	// Field of View
	float fDepth;		// Maximum rendering distance
	float fSpeed;			// Walking Speed

	float *fDepthBuffer = new float[ScreenWidth()];

	void DrawMode() {

		//ClearScreen();
		//DrawRDPRect({0,0},{256,100},CYAN);
		for (uint32_t x = 0; x < ScreenWidth(); x++)
		{
			// For each column, calculate the projected ray angle into world space
			float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)ScreenWidth()) * fFOV;

			// Find distance to wall
			float fStepSize = 0.1f;	  // Increment size for ray casting, decrease to increase	
			float fDistanceToWall = 0.0f; //                                      resolution
			float fDistanceToObj = 0.0f;

			bool bHitWall = false;		// Set when ray hits wall block
			bool bHitObj = false;
			//bool bBoundary = false;		// Set when ray hits boundary between two wall blocks

			float fEyeX = sinf(fRayAngle); // Unit vector for ray in player space
			float fEyeY = cosf(fRayAngle);

			float fSampleX = 0.0f;
			//float fSampleY = 0.0f;
			//bool bLit = false;

			// Incrementally cast ray from player, along ray angle, testing for 0x00008E00
			// intersection with a block
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

						//DrawBox({nTestX, nTestY}, 4, RED);

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
						// Ray has hit wall
						bHitObj = true;

						//DrawBox({nTestX, nTestY}, 4, ORANGE);


					}
				}
			}

			// Calculate distance to ceiling and floor
			int nCeiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)fDistanceToWall);
			int nObjCeiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)fDistanceToObj);
			
			int nFloor = ScreenHeight() - nCeiling;
			int nObjFloor = ScreenHeight() - nObjCeiling - 40;
			// Update Depth Buffer
			fDepthBuffer[x] = fDistanceToWall;
	
			
			for (int y = 0; y < (int)ScreenHeight(); y++)
			{
				if (y <= nCeiling) {
					float fSampleY = ((float)y - 0);
					int col = sky->data[int(fSampleY * 5.5 + fSampleX + fPlayerA)];
					Graphics::DrawPixel(this->d, x,y, col);
					
				}
				else if (y > nCeiling && y <= nFloor)
				{
					// Draw Wall
					float fSampleY = ((float)y - (float)nCeiling) / ((float)nFloor - (float)nCeiling);
					int col = wall->data[int(fSampleY * (wall->width*wall->height) + fSampleX)];
				
					if (fDistanceToWall > 0 && fDistanceToWall < 14)
					{
					//	if(get_pixel(this->d, x, y) != 0x909090FF)
	
							DrawPixel({x,y}, col * fDistanceToWall * 0.0002);
					}
					else if (fDistanceToWall > 14) {
						DrawPixel({x,y}, 0X0);
					}
				}
				else // Floor
				{
					float b = 1.0f - y / ScreenHeight() / 2;
					float fSampleY = ((float)y) - fPlayerA / ScreenWidth();

					//int col = grass->data[int(fSampleY * 1 + fSampleX)];
					DrawPixel({x, y}, 0x909090FF);
				}
			
				if (y > nObjCeiling && y <= nObjFloor) {
					// Draw Wall
									// Draw Wall
					if (fDistanceToObj >= 10 && fDistanceToObj <= 14) {
					//
					}
					if (fDistanceToObj >= 6 && fDistanceToObj <= 10)
					{
					//	DrawPixel({x,y}, DEEP_DARK_RED);
					}
					if (fDistanceToObj > 0 && fDistanceToObj < 6)
					{
						//DrawPixel({x,y}, RED);
					}
			
				}
			}

		}
			Graphics::SetColor(RED, DARK_GREY);
			DrawTextFormat({0,(int)(ScreenHeight()/2)+40},"Player\nX: %0.2f Y: %0.2f\nA: %0.2f", fPlayerX, fPlayerY, fPlayerA);
			DrawTextFormat({0,(int)(ScreenHeight()/2)+70},"%08x %08x %08x %08x", 
			__safe_buffer[0], __safe_buffer[1], __safe_buffer[2], __safe_buffer[3]
			);
		
	}
	
	virtual void OnCreate() override
	{
		nMapWidth = 32;
		nMapHeight = 32;
		fPlayerX = 12.0f;			// Player Start Position
		fPlayerY = 8;// 5.09f;
		fPlayerA = -3.14159f / 2.0f;			// Player Start Rotation
		fFOV = 3.14159f / 3.0f;	// Field of View
		fDepth = 16.0f;			// Maximum rendering distance
		fSpeed = 1.0f;			// Walking Speed

		wall  = DFS::QuickRead<sprite_t*>("/wall.sprite");
		sky   = DFS::QuickRead<sprite_t*>("/sky.sprite");
		grass = DFS::QuickRead<sprite_t*>("/grass.sprite");

	}

protected:
	virtual void KeyDLPressed() override {
		fPlayerA -= (fSpeed * 0.2f);
		DrawMode();

	}

	virtual void KeyDRPressed() override {
		fPlayerA += (fSpeed * 0.2f);
		DrawMode();
	}

	virtual void KeyDUPressed() override
	{
		fPlayerX += sinf(fPlayerA) * fSpeed;
		fPlayerY += cosf(fPlayerA) * fSpeed;
		if (map[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
		{
			fPlayerX -= sinf(fPlayerA) * fSpeed;
			fPlayerY -= cosf(fPlayerA) * fSpeed;
		}
		DrawMode();
	}

	virtual void KeyDDPressed() override
	{
		fPlayerX -= sinf(fPlayerA) * fSpeed;
		fPlayerY -= cosf(fPlayerA) * fSpeed;
		if (map[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
		{
			fPlayerX += sinf(fPlayerA) * fSpeed;
			fPlayerY += cosf(fPlayerA) * fSpeed;
		}
		DrawMode();
	}

	virtual void KeyAPressed() override {

	}

	virtual void KeyBPressed() override {
		fFar -= 0.1;
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
	Mode7Test n64g(RESOLUTION_256x240, DEPTH_32_BPP, ANTIALIAS_OFF, Frame::UIType::GUI);
	n64g.Begin();

	return 0;
}
