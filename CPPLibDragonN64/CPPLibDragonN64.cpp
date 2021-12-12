#include <stdio.h>
#include <stdlib.h>
#include "LibN64.h"
/*  virtual functions to override:
    KeyAPressed
    KeyBPressed
    KeyDUPressed
    KeyDDPressed
    KeyDLPressed
    KeyDRPressed
    KeyZPressed
    OnCreate
    FrameUpdate
*/

using namespace LibN64;

class Instance : public Frame {
public:
    Instance(resolution_t res, bitdepth_t dep) : Frame(res, dep) {}

    virtual void OnCreate() override 
    {

        romTitle = "LibN64 Test";
    }
    
protected:

    virtual void FrameUpdate() override
    {	
		auto *buf = DFS::QuickRead("/data.txt");
	
		DrawText(5,10,buf);
	
        DrawCircle(30, 60, 6, RED);
        DrawTextFormat(30, 90, "Total %0.2f Elapsed %0.2f", Ticks2Seconds(fTotalTime), TICKS2SECONDS(fFrameTime));

    }
    
    virtual void KeyAPressed() override {
        DrawText(10, 30, "Key A pressed.");
    }

    virtual void KeyBPressed() override {
        ClearScreen();
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
    Instance n64g(RESOLUTION_320x240, DEPTH_32_BPP);
    n64g.Begin();
   
    return 0;
}
