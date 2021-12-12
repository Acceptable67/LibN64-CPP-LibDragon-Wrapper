#include <stdio.h>
#include <stdlib.h>
#include "LibN64.h"
#include "LibN64GameTest.h"

#define DARK_GREY graphics_make_color(136, 138, 137, 255)
#define VERY_DARK_GREY graphics_make_color(77, 79, 77, 255)

/*test of pseudo-3D planes on LibDragon.*/

const char *map = "################################" 
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
"###..####....########....#######"
"####.####.......######.........."
"#...............#..............."
"#.......#########.......##..####"
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
	Mode7Test(resolution_t res, bitdepth_t dep) : Frame(res, dep) {}

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

	float fDepthBuffer[400];

	void DrawMode() {

		//ClearScreen();
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

			// Incrementally cast ray from player, along ray angle, testing for 
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


						DrawBox(nTestX, nTestY, 4, RED);

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

						DrawBox(nTestX, nTestY, 4, ORANGE);


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
	
			rdp_init();
			rdp_set_default_clipping();
	
			for (int y = 0; y < (int)ScreenHeight(); y++)
			{
				if (y <= nCeiling) {
					graphics_draw_pixel(d, x, y, CYAN);
			
				}
				else if (y > nCeiling && y <= nFloor)
				{
					// Draw Wall
					float fSampleY = ((float)y - (float)nCeiling) / ((float)nFloor - (float)nCeiling);
					int col = checkerwall[int(fSampleY * 2 + fSampleX)];
					if (fDistanceToWall >= 10 && fDistanceToWall <= 14) {
						graphics_draw_pixel(d, x, y, col - 0x40404000);
						//graphics_draw_pixel(d, x, y, 0x303030FF);
					}
					if (fDistanceToWall >= 6 && fDistanceToWall <= 10)
					{
						graphics_draw_pixel(d, x, y, 0x585858FF);
					}
					if (fDistanceToWall > 0 && fDistanceToWall < 6)
					{
						graphics_draw_pixel(d, x, y, 0x909090FF);
					}
					else if (fDistanceToWall > 14) {
						graphics_draw_pixel(d, x, y, 0X0);
					}
				}
				else // Floor
				{
					//float b = 1.0f - (((float)y - ScreenWidth() / 2.0f) / ((float)ScreenHeight()  / 2.0f));
					DrawBox(x, y, 1, DARK_GREEN);
				}
			
				if (y > nObjCeiling && y <= nObjFloor) {
					// Draw Wall
									// Draw Wall
					if (fDistanceToObj >= 10 && fDistanceToObj <= 14) {
						graphics_draw_pixel(d, x, y, DEEP_DARK_RED);
					}
					if (fDistanceToObj >= 6 && fDistanceToObj <= 10)
					{
						graphics_draw_pixel(d, x, y, DEEP_DARK_RED);
					}
					if (fDistanceToObj > 0 && fDistanceToObj < 6)
					{
						graphics_draw_pixel(d, x, y, RED);
					}
			
				}
			}

		}
		for (int nx = 0; nx < nMapWidth; nx += 3)
			for (int ny = 0; ny < nMapHeight; ny += 3)
				DrawTextFormat<char>(nx, ny, (char*)"%c", map[ny * nMapWidth + nx]);
		//DrawTextFormat<char>(3 + (int)fPlayerY, 3 + (int)fPlayerX, (char*)"%c", 'P');

		//int* fBufWidth = (int*)(0xA4100008);
		//char buffer[20];
		//sprintf(buffer, "0x80000400: %08X", *fBufWidth);
		//graphics_draw_text(this->d, 20, 120, buffer);
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
	Mode7Test n64g(RESOLUTION_320x240, DEPTH_32_BPP);
	n64g.Begin();

	return 0;
}
