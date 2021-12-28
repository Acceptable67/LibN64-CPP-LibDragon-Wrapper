#pragma GCC diagnostic ignored "-Wwrite-strings"

/*TEST OF LIBSCRIPT + NEW LIB FEATURES*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <vector>

#include "LibN64.h"
#include "LibString.h"
#include "LibVector.h"
#include "LangParser.h"

using namespace LibN64;

class Parser : public Frame {
public:
	Parser(resolution_t res, bitdepth_t dep, antialias_t aa, UIType ui) : Frame(res, dep, aa, ui) {}

private:
	LangParser p;
//	LibVector<byte> vec;
 


protected:
	virtual void OnCreate() override 
	{

   		debug_init(DEBUG_FEATURE_ALL);
		LibString file = { "/data.lbs" };
		
   		auto code = DFS::QuickRead<char*>(file);
		
		//debugf("%02x at line %d", vec.at(0), __LINE__);

		if(code)
			p.Parse(*this, code, DFS::Size());
	}
	
	virtual void KeyBPressed() override 
	{
		ClearScreen();
		OnCreate();
	}
	virtual void KeyZPressed() override
	{
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
	Parser n64g(RESOLUTION_320x240, DEPTH_32_BPP, ANTIALIAS_RESAMPLE, Frame::UIType::GUI);
	n64g.Begin();




	return 0;
}
