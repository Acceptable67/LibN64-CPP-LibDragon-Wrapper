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


# Descriptions and images
### LibN64GameTest
Pseudo-3D Textured Raycaster FPS experiment, still working out the quirks.

<img src="http://73.55.44.55/github/CPPWrapper-LibN64.png"></img>
    
### Mode7 Test
Roam across a pseudo-3D landscape with a texture of a mountain on it. A test of 'old school' pseudo-3D planes.

<img src="http://73.55.44.55/github/CPPWrapper-Mode73D.png"></img>
    
### MemPakTesting
Test of LibMemPak class interface 

<img src="http://73.55.44.55/github/CPPWrapper-LibMemPak.png"></img>
    
### MazeTest
Auto-generated maze that auto fills it self. Takes around 30 seconds to fully finish.

<img src="http://73.55.44.55/github/CPPWrapper-Maze.png"></img>

### PanAndZoom
A test of panning and zooming around a white Grid with ability to fill in the spaces inside with a circle (Screen-to-World mapping and vice versa.)

<img src="http://73.55.44.55/github/CPPWrapper-PanAndZoom.png"></img>

### Asteroids
It's a game of Asteroids!

<img src="http://73.55.44.55/github/CPPWrapper-Asteroids.png"></img>

### 3D Model Viewer
3D Model viewer with ability to move camera around the object

<img src="http://73.55.44.55/github/CPPWrapper-3DViewer.png"></img>

### CirclesAndSquares
Auto-random generation of squares and snowflake-like circles

<img src="http://73.55.44.55/github/CPPWrapper-CANDS.png"></img>

### MenuTesting
Test of rudimentary menu system

<img src="http://73.55.44.55/github/CPPWrapper-Menu.png"></img>

### CPPLibDragon
A demo of the LibN64 Class structure.

<img src="http://73.55.44.55/github/CPPWrapper-CPPLibDragon.png"></img>

### LangParser
Demo of a WIP 'LibScript' parser. LibScript is an experimental custom scripting language I am playing around with.

<img src="http://73.55.44.55/github/CPPWrapper-LangParser.png"></img>
