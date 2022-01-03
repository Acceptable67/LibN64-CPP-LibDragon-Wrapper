#include <stdio.h>
#include <sstream>
#include "CPPTesting.h"

#include <memory>
#include <typeinfo>
#include <bitset>
#include <algorithm>
#include <chrono>
#include <ctime>

using namespace std;

float fElapsedTime = 0.020000;

void CPPTest::KeyAPressed() {
vecBullets.push_back({ 0, player.x, player.y, 50.0f * sinf(player.angle), -50.0f * cosf(player.angle), 100.0f });
}
void CPPTest::KeyBPressed() {
    num--;
}

void CPPTest::KeyZPressed() {
	activeshape = ~activeshape & 1;
}

void CPPTest::KeyDUPressed() {
		player.dx += sin(player.angle) * 20.0f * fElapsedTime;
			player.dy += -cos(player.angle) * 20.0f * fElapsedTime;
	
}
void CPPTest::KeyDDPressed() {
		player.x += player.dx * fElapsedTime;
		player.y += player.dy * fElapsedTime;
}
void CPPTest::KeyDLPressed() {
		player.angle -= 5.0f * fElapsedTime;
		
}
void CPPTest::KeyDRPressed() {
	player.angle += 5.0f * fElapsedTime;
}

// Implements "wrap around" for various in-game sytems
void CPPTest::WrapCoordinates(float ix, float iy, float &ox, float &oy)
	{
		ox = ix;
		oy = iy;
		if (ix < 0.0f)	ox = ix + (float)ScreenWidth();
		if (ix >= (float)ScreenWidth())	ox = ix - (float)ScreenWidth();
		if (iy < 0.0f)	oy = iy + (float)ScreenHeight();
		if (iy >= (float)ScreenHeight()) oy = iy - (float)ScreenHeight();
}

	// Overriden to handle toroidal drawing routines
	
void CPPTest::DrawPixel(LibPos pos, unsigned color)
{
	float fx, fy;
	WrapCoordinates(pos.x, pos.y, fx, fy);	
	Frame::DrawPixel({(int)fx,(int)fy}, color);
}	


bool CPPTest::IsPointInsideCircle(float cx, float cy, float radius, float x, float y)
	{
		return sqrt((x-cx)*(x-cx) + (y-cy)*(y-cy)) < radius;
	}

void CPPTest::FrameUpdate()
{

		if (bDead)
			ResetGame();

		ClearScreen();	

		player.x += player.dx * fElapsedTime;
		player.y += player.dy * fElapsedTime;
		// Keep ship in gamespace
		WrapCoordinates(player.x, player.y, player.x, player.y);

		// Check ship collision with asteroids
		for (auto &a : vecAsteroids)
			if (IsPointInsideCircle(a.x, a.y, a.nSize, player.x, player.y))
				bDead = true; // Uh oh...

		// Update and draw asteroids
		for (auto &a : vecAsteroids)
		{
			a.x += a.dx * fElapsedTime;
			a.y += a.dy * fElapsedTime;
			a.angle += 0.5f * fElapsedTime; // Add swanky rotation :)

			// Asteroid coordinates are kept in game space (toroidal mapping)
			WrapCoordinates(a.x, a.y, a.x, a.y);

			DrawWireFrameModel(vecModelAsteroid, a.x, a.y, a.angle, (float)a.nSize, YELLOW);	
		}

		vector<sSpaceObject> newAsteroids;

// Update Bullets
		for (auto &b : vecBullets)
		{
			b.x += b.dx * fElapsedTime;
			b.y += b.dy * fElapsedTime;
			WrapCoordinates(b.x, b.y, b.x, b.y);
			b.angle -= 1.0f * fElapsedTime;

			// Check collision with asteroids
			for (auto &a : vecAsteroids)
			{
				//if (IsPointInsideRectangle(a.x, a.y, a.x + a.nSize, a.y + a.nSize, b.x, b.y))
				if(IsPointInsideCircle(a.x, a.y, a.nSize, b.x, b.y))
				{
					b.x = -100;

					// Create child asteroids
					if (a.nSize > 4)
					{
						float angle1 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
						float angle2 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
						newAsteroids.push_back({ (int)a.nSize >> 1 ,a.x, a.y, 10.0f * sinf(angle1), 10.0f * cosf(angle1), 0.0f });
						newAsteroids.push_back({ (int)a.nSize >> 1 ,a.x, a.y, 10.0f * sinf(angle2), 10.0f * cosf(angle2), 0.0f });
					}

					// Remove asteroid - Same approach as bullets
					a.x = -100;
					nScore += 100; // Small score increase for hitting asteroid
				}
			}
		}

		// Append new asteroids to existing vector
		for(auto a:newAsteroids)
			vecAsteroids.push_back(a);

		// Clear up dead objects - They are out of game space

		// Remove asteroids that have been blown up
		if (vecAsteroids.size() > 0)
		{
			auto i = remove_if(vecAsteroids.begin(), vecAsteroids.end(), [&](sSpaceObject o) { return (o.x < 0); });
			if (i != vecAsteroids.end())
				vecAsteroids.erase(i);
		}

		if (vecAsteroids.empty()) // If no asteroids, level complete! :) - you win MORE asteroids!
		{
			// Level Clear
			nScore += 1000; // Large score for level progression
			vecAsteroids.clear();
			vecBullets.clear();

			// Add two new asteroids, but in a place where the player is not, we'll simply
			// add them 90 degrees left and right to the player, their coordinates will
			// be wrapped by th enext asteroid update
			vecAsteroids.push_back({ (int)16, 30.0f * sinf(player.angle - 3.14159f/2.0f) + player.x,
											  30.0f * cosf(player.angle - 3.14159f/2.0f) + player.y,
											  10.0f * sinf(player.angle), 10.0f*cosf(player.angle), 0.0f });

			vecAsteroids.push_back({ (int)16, 30.0f * sinf(player.angle + 3.14159f/2.0f) + player.x,
											  30.0f * cosf(player.angle + 3.14159f/2.0f) + player.y,
											  10.0f * sinf(-player.angle), 10.0f*cosf(-player.angle), 0.0f });
		}

		// Remove bullets that have gone off screen
		if (vecBullets.size() > 0)
		{
			auto i = remove_if(vecBullets.begin(), vecBullets.end(), [&](sSpaceObject o) { return (o.x < 1 || o.y < 1 || o.x >= ScreenWidth() - 1 || o.y >= ScreenHeight() - 1); });
			if (i != vecBullets.end())
				vecBullets.erase(i);
		}

		// Draw Bullets
		for (auto b : vecBullets)
			this->DrawPixel({(int)b.x, (int)b.y}, WHITE);

		// Draw Ship
		DrawWireFrameModel(vecModelShip, player.x, player.y, player.angle);

		// Draw Score
		DrawTextFormat({2, 2}, "SCORE: %d", nScore);
}

void CPPTest::DrawWireFrameModel(const vector<pair<float, float>> &vecModelCoordinates, float x, float y, float r, float s, unsigned col) {
// Create translated model vector of coordinate pairs
		vector<pair<float, float>> vecTransformedCoordinates;
		int verts = vecModelCoordinates.size();
		vecTransformedCoordinates.resize(verts);

		// Rotate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecModelCoordinates[i].first * cosf(r) - vecModelCoordinates[i].second * sinf(r);
			vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf(r) + vecModelCoordinates[i].second * cosf(r);
		}

		// Scale
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * s;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;
		}

		// Translate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
		}

		// Draw Closed Polygon
		for (int i = 0; i < verts + 1; i++)
		{
			int j = (i + 1);
			DrawLine({(int)vecTransformedCoordinates[i % verts].first, (int)vecTransformedCoordinates[i % verts].second}, 
				{(int)vecTransformedCoordinates[j % verts].first, (int)vecTransformedCoordinates[j % verts].second}, col);
		}
	
}

void CPPTest::ResetGame()
	{
		// Initialise Player Position
		player.x = ScreenWidth() / 2.0f;
		player.y = ScreenHeight() / 2.0f;
		player.dx = 0.0f;
		player.dy = 0.0f;
		player.angle = 0.0f;

		vecBullets.clear();
		vecAsteroids.clear();

		// Put in two asteroids
		vecAsteroids.push_back({ (int)16, 20.0f, 20.0f, 8.0f, -6.0f, 0.0f });
		vecAsteroids.push_back({ (int)16, 100.0f, 20.0f, -5.0f, 3.0f, 0.0f });

		// Reset game
		bDead = false;
		nScore = false;
	}

auto CPPTest::OnCreate() -> void
{
		vecModelShip = 
		{
			{ 0.0f, -5.0f},
			{-2.5f, +2.5f},
			{+2.5f, +2.5f}
		}; // A simple Isoceles Triangle

		// Create a "jagged" circle for the asteroid. It's important it remains
		// mostly circular, as we do a simple collision check against a perfect
		// circle.
		int verts = 20;
		for (int i = 0; i < verts; i++)
		{
			float noise = (float)rand() / (float)RAND_MAX * 0.4f + 0.8f;
			vecModelAsteroid.push_back(make_pair(noise * sinf(((float)i / (float)verts) * 6.28318f), 
												 noise * cosf(((float)i / (float)verts) * 6.28318f)));
		}

		ResetGame();
	
}


int main(void) {
	CPPTest n64g(RESOLUTION_256x240, DEPTH_32_BPP, GUI);
	n64g.Begin();
	return 0;
}
