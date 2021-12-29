# LibN64 C++ LibDragon Wrapper
A C++ wrapper for libdragon development that allows an easy to use interface to create libdragon C++ ROMs. After using a 2010 build of LibDragon with GCC 4.1 for so many years, now fully supports the latest build with C++20

#### Install
Place LIbN64.cpp in the src directory and LibN64.h in the include directory, reconfigure your LibDragon makefile to accept these new files. Build, make install, include <LibN64.h> in your project and should be good to go.

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
    
    virtual void KeyJoyPressed(int data) {
        switch(data) {
            case Joystick::JoyUp: /*action*/ break;
            //so on...
            default: break;
        };
    }
    
    virtual void KeyAPressed() override {

    }

    virtual void KeyBPressed() override {

    }
    
    virtual void KeyZPressed() override {

    }
};

int main(void) {
    Instance n64g(RESOLUTION_320x240, DEPTH_32_BPP, ANTIALIAS_RESAMPLE, Frame::UIType::GUI);
    n64g.Begin();
   
    return 0;
}
```


# Descriptions and images
### LibN64GameTest
Pseudo-3D Textured Raycaster FPS experiment, still working out the quirks.

<img src="http://73.55.44.55/github/CPPWrapper-LibN64.png"></img>

### MazeTest
Auto-generated maze that auto fills it self. Takes around 30 seconds to fully finish.

<img src="http://73.55.44.55/github/CPPWrapper-Maze.png"></img>

### 3D Model Viewer
3D Model viewer with ability to move camera around the object

<img src="http://73.55.44.55/github/CPPWrapper-3DViewer.png"></img>


### CPPLibDragon
A demo of the LibN64 Class structure.

<img src="http://73.55.44.55/github/CPPWrapper-CPPLibDragon.png"></img>

### LangParser
Demo of a WIP 'LibScript' parser. LibScript is an experimental custom scripting language I am playing around with.

<img src="http://73.55.44.55/github/CPPWrapper-LangParser.png"></img>
