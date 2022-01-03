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

class Instance : public Frame {
 
public:
    Instance(resolution_t res, bitdepth_t dep, antialias_t aa, UIType ui) : Frame(res, dep, aa, ui) {}

    virtual void OnCreate() override 
    {
        
        GameTitle = "LibN64 Test";
        SetKeyState(KeysDown);

    }

protected:
    const std::string file = { "/data.txt" };

    bool pressedY = false;

    float y = 50;
    display_context_t activeDisplay = d;
    virtual void FrameUpdate() override
    {	
        ClearScreenRDP();
		const std::string buf = DFS::QuickRead<char*>(file.c_str());

    
        DrawRect({20,20},{100,50}, SKY_BLUE, false);
        DrawRDPTri({10,160},{20,180},{40,170}, Color::ORANGE);
        DrawTriangle({10,120},{20,140},{40,130},Color::RED);
		DrawText({5,10},buf.c_str());
        graphics_draw_text(this->d2, 50, 35, "test");
        	graphics_fill_screen(d2, 0xffffffff);
    
        DrawTextFormat({30,90}, "Total   %0.4f\nElapsed %0.4f Rate %0.4f",GetTotalTime(), GetFrameTime(),
        GetFrameRate());
         DrawCircle({30, 60}, 8, 0.4, SKY_BLUE, false);
        
    }
    
    virtual void KeyJoyYPressed(int dir) override {
        char* direction = nullptr;
        switch(dir) 
        {
            case Joystick::JoyUp       : SwitchBuffer(d2); display_show(d2); break;
            case Joystick::JoyDown     : SwitchBuffer(d); display_show(d); break;
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

    Instance n64g(RESOLUTION_320x240, DEPTH_32_BPP, ANTIALIAS_RESAMPLE, Frame::UIType::GUI);
    n64g.Begin();
   
    return 0;
}
