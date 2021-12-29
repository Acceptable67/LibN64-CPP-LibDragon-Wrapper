
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

enum 
class CommandType 
{
    DRAWTEXT,
    DRAWPIXEL
};

int comnum = 2;

class CPPTest : public Frame  {

	   class LibCommand {
        public:
        int            id;
        CommandType    com;
        Frame::LibPos  pos;
        bool finished = false; 
        virtual void Execute() {}
    };

    class LibCommand_DrawText : public LibCommand
    {
        public:
        std::string text;
        Frame *ref;
            LibCommand_DrawText(LibN64::Frame& r, int i, Frame::LibPos p, std::string t) {
                com = CommandType::DRAWTEXT;
                pos = p;
                text = t;
                id = i;
                ref = &r;
            }

        virtual void Execute() override
        {
            ref->DrawText(pos, text.c_str());
        }
    };

    std::vector<LibCommand*> CommandList;

 public:
   CPPTest(resolution_t res, bitdepth_t dep, antialias_t aa, UIType ui) : Frame(res, dep, aa, ui) {}

	
   virtual void OnCreate() {
		CommandList.push_back(new LibCommand_DrawText(*this, comnum++,Frame::LibPos{0,0}, "test one"));

	
		DrawTextFormat({0,20},"Buffer: %d");

   }

   virtual void FrameUpdate() {
	   
	    CommandList[0]->Execute();
   }

   virtual void KeyJoyPressed(int data) 
   {
	   ClearScreenRDP();
	    switch(data) 
		{
			case Joystick::JoyUp:  DrawText({0,0}, "joy up"); break;
			case Joystick::JoyDown: DrawText({0,20}, "joy down");break;
		}
   }
};


int main(void) {
	CPPTest n64g(RESOLUTION_320x240, DEPTH_16_BPP, ANTIALIAS_RESAMPLE, Frame::UIType::GUI);
	n64g.Begin();
	return 0;
}
