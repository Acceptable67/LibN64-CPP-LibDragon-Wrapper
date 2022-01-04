
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <map>
#include <set>
#include <stack>

#include <functional>

#include <LibN64.h>

using namespace LibN64;

using namespace std;

class CPPTest : public Frame  {
private:
	struct sSpaceObject
	{
		int nSize;
		float x;
		float y;
		float dx;
		float dy;
		float angle;
	};

	vector<sSpaceObject> vecAsteroids;
	vector<sSpaceObject> vecBullets;
	sSpaceObject player;
	bool bDead = false;
	int nScore = 0;

	vector<pair<float, float>> vecModelShip;
	vector<pair<float, float>> vecModelAsteroid;
 public:
   CPPTest(resolution_t res, bitdepth_t dep, antialias_t aa, Frame::UIType ui) : Frame(res, dep, aa, ui) {}

   void DrawingArea();
   void ResetGame();

	 virtual void DrawPixel(LibPos pos, unsigned c);
	void WrapCoordinates(float ix, float iy, float &ox, float &oy);
  bool IsPointInsideCircle(float cx, float cy, float radius, float x, float y);
  void DrawWireFrameModel(const vector<pair<float, float>> &vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, unsigned col = WHITE);
   virtual void OnCreate() override;
   virtual void FrameUpdate() override;


   virtual void KeyDUPressed() override;
   virtual void KeyDDPressed() override;
   virtual void KeyDLPressed() override;
   virtual void KeyDRPressed() override;
   virtual void KeyZPressed() override;

   virtual void KeyAPressed() override;
   virtual void KeyBPressed() override;
    //  virtual void KeyZPressed() override;

     //    virtual void KeyJoyXPressed() override;
     // virtual void KeyJoyYPressed() override;
   //virtual void __OnLoop_FreeFunction2() override;
  // virtual void __OnInit_FreeFunction1() override;

   std::function<void()> wrapper;
   friend void incNum(CPPTest& r);
};