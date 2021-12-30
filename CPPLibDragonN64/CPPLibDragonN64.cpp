#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <functional>
#include <thread>
#include <LibN64.h>
#include "LibString.h"
#include "LibVector.h"
#include "LibMap.h"
#include "LibSprite.h"

/*  virtual functions to override:
    KeyAPressed  KeyZPressed
    KeyBPressed 
    KeyDUPressed KeyCUPressed
    KeyDDPressed KeyCDPressed
    KeyDLPressed KeyCLPressed
    KeyDRPressed KeyCRPressed
    KeyJoyPressed
    KeyStartPressed
    OnCreate      __OnInit_FreeFunction1
                  __OnInit_FreeFunction2
    FrameUpdate   __OnLoop_FreeFunction1 
                  __OnLoop_FreeFunction2
*/

using namespace LibN64;


double t3 = 0;

void threa()
{

}

class Instance : public Frame {
 
public:
    Instance(resolution_t res, bitdepth_t dep, antialias_t aa, UIType ui) : Frame(res, dep, aa, ui) {}

    virtual void OnCreate() override 
    {
        
        romTitle = "LibN64 Test";
  
    }

protected:
    const std::string file = { "/data.txt" };

    virtual void __OnLoop_FreeFunction1() override
    {
        timer_init();
        new_timer(TIMER_TICKS(10000), TF_CONTINUOUS, [](int ovfl) 
        {
            t3+=0.001;
        });
    }

     virtual void __OnLoop_FreeFunction2() override
     {
         fFrameTime = timer_ticks();
		 fTotalTime += fFrameTime;
		 timer_close();
     }

    virtual void FrameUpdate() override
    {	
        if(t3 > 0.008) { ClearScreenRDP();  t3 = 0; }
		const std::string buf = DFS::QuickRead<char*>(file.c_str());


        DrawRDPTri({10,160},{20,180},{40,170}, RED);
        DrawTriangle({10,120},{20,140},{40,130},RED);
		DrawText({5,10},buf.c_str());
        DrawCircle({30, 60}, 6, RED);
        DrawTextFormat({30,90}, "Total %0.2f Elapsed %0.2f t3 %0.2f", Ticks2Seconds(fTotalTime), TICKS2SECONDS(fFrameTime), t3);

    }
    
    virtual void KeyJoyYPressed(int dir) override {
        char* direction = nullptr;
        switch(dir) 
        {
            case Joystick::JoyUp       : direction = "up";   break;
            case Joystick::JoyDown     : direction = "down"; break;

            default: break;
        }
        DrawTextFormat({40,100},"Data %08X Direction %s", dir, direction);
        
    }

        virtual void KeyJoyXPressed(int dir) override {
        char* direction = nullptr;
        switch(dir) 
        {
            case Joystick::JoyLeft     : direction = "left"; break;
            case Joystick::JoyRight    : direction = "right";break;

            default: break;
        }
        DrawTextFormat({40,80},"Data %08X Direction %s", dir, direction);
        
    }


    virtual void KeyAPressed() override {
        DrawText({10,30}, "Key A pressed.");
    }

    virtual void KeyBPressed() override {
        ClearScreenRDP();
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

    Instance n64g(RESOLUTION_320x240, DEPTH_32_BPP, ANTIALIAS_OFF, Frame::UIType::GUI);
    n64g.Begin();
   
    return 0;
}
