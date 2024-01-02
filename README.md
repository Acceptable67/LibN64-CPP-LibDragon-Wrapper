# LibN64 C++ LibDragon Wrapper
A C++ wrapper for libdragon development that allows an easy to use interface to create libdragon C++ ROMs. After using a 2010 build of LibDragon with GCC 4.1 for so many years, now fully supports the latest build with C++20

#### Install
Place <i>LibN64.h</i> in the include directory, reconfigure your LibDragon makefile to install the new header. Make install, include <i><LibN64.h></i> in your project and should be good to go.

#### Documentation
Build the new documentation with Doxygen from the root Libdragon folder. All the classes and their features will be highlighted in there.

### Basic Class Template
```c++
#include <stdio.h>
#include <stdlib.h>
#include <LibN64.h>

using namespace LibN64;

class Instance : public Frame {
public:
    Instance(resolution_t res, bitdepth_t dep, antialias_t aa, UIType ui) : Frame(res, dep, aa, ui) {}

    virtual void OnCreate() override 
    {
        //called once program starts
    }
    
protected:

    virtual void FrameUpdate() override
    {	
        //every frame
    }
    
    virtual void KeyJoyYPressed(int data) {
        switch(data) {
            case Joystick::JoyUp: /*action*/ break;
            //...
            default: break;
        };
    }
    
    virtual void KeyJoyXPressed(int data) {
        switch(data) {
            case Joystick::JoyLeft: /*action*/ break;
            //...
            default: break;
        };
    }
    
    
    virtual void KeyAPressed() override {

    }

    virtual void KeyBPressed() override {

    }
    
    virtual void KeyZPressed() override {

    }
    /*and so on*/
};

int main(void) {
    Instance n64g(RESOLUTION_320x240, DEPTH_32_BPP, ANTIALIAS_RESAMPLE, Frame::UIType::GUI);
    n64g.Begin();
   
    return 0;
}
```
