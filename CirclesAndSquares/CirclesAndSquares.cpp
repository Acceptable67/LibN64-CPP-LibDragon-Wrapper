
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

class CPPTest : public Frame  {
 public:
   CPPTest(resolution_t res, bitdepth_t dep, antialias_t aa, UIType ui) : Frame(res, dep, aa, ui) {}

	int shapesDrawn;
protected:
	sprite_t *font = nullptr;

	void OnCreate() override 
	{
		//LoadCustomFont("/font.sprite");
		
	}
	

	void FrameUpdate() override 
	{ 
		DrawCircle({rand() % ScreenWidth() + 0, rand() % ScreenHeight() + 0},  rand() % 16 + 1,  rand() % 6  + 2,  rand() % 0xFFFFFFFF, rand() % 2);
		DrawRect  ({rand() % ScreenWidth() + 0, rand() % ScreenHeight() + 0}, {rand() % 30 + 20, rand() % 30 + 20}, rand() % 0xFFFFFFFF);
		//shapesDrawn+=2;
	
	}


};

int main(void) {
	CPPTest n64g(RESOLUTION_320x240, DEPTH_32_BPP, ANTIALIAS_OFF, Frame::UIType::GUI);
	n64g.Begin();
	
	return 0;
}
