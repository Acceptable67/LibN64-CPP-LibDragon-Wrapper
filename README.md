# LibN64 C++ LibDragon Wrapper
A C++ wrapper for libdragon development that allows an easy to use interface to create libdragon C++ ROMs.
After using a 2010 build of LibDragon with GCC 4.1 for so many years, now fully supports the latest build with C++20

## Basic Class Template

```c++
#include <stdio.h>
#include <stdlib.h>
#include "LibN64.h"

using namespace LibN64;

class Instance : public Frame {
public:
    Instance(resolution_t res, bitdepth_t dep) : Frame(res, dep) {}

    virtual void OnCreate() override 
    {
        //called once program starts
    }
    
protected:

    virtual void FrameUpdate() override
    {	
        //every frame
    }
    
    virtual void KeyAPressed() override {

    }

    virtual void KeyBPressed() override {

    }
    
    virtual void KeyZPressed() override {

    }
};

int main(void) {
    Instance n64g(RESOLUTION_320x240, DEPTH_32_BPP);
    n64g.Begin();
   
    return 0;
}
```
