#pragma GCC diagnostic ignored "-Wwrite-strings"

/*TEST OF LIBSCRIPT + NEW LIB FEATURES*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "LibN64.h"
#include "LibString.h"
#include "LibVector.h"
#include "LangParser.h"

using namespace LibN64;

class Parser : public Frame {
public:
	Parser(resolution_t res, bitdepth_t dep, int ui) : Frame(res, dep, ui) {}

private:
	LangParser p;
protected:
	virtual void OnCreate() override 
	{		
   		debug_init(DEBUG_FEATURE_ALL);
		LibString file = { "/data.lbs" };
		
   		auto code = DFS::QuickRead(file);
		if(code)
			p.Parse(this, code, DFS::Size(file));
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
	Parser n64g(RESOLUTION_320x240, DEPTH_32_BPP, GUI);
	n64g.Begin();

	return 0;
}
