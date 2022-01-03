
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
#include <any>
#include <list>
#include <LibN64.h>

double timer;

using namespace LibN64;

class CPPTest : public LibN64::Frame  {
 public:
   CPPTest(resolution_t res, bitdepth_t dep, antialias_t aa, UIType ui) : Frame(res, dep, aa, ui) {}
/*	float x = 120, y = 120, angle = 0;
	float px = 50, py = 55;

	float vx1 = 70; float vy1 = 20;
	float vx2 = 70; float vy2 = 70;

	void _DrawBox() {
		DrawRDPRect({100,20},{238,75}, ORANGE);
		DrawTextFormat({100,20}, "=================", "str");
		DrawText({100,25}, "="); DrawText({228,25}, "="); 
		DrawText({100,30}, "=");DrawText({228,30}, "="); 
		DrawText({100,35}, "=");DrawText({228,35}, "="); 
		DrawText({100,40}, "=");DrawText({228,40}, "="); 
		DrawText({100,45}, "=");DrawText({228,45}, "="); 
		DrawText({100,50}, "=");DrawText({228,50}, "="); 
		DrawText({100,55}, "=");DrawText({228,55}, "="); 
		DrawText({100,60}, "=");DrawText({228,60}, "="); 
		DrawTextFormat({100,65}, "=================", "str");
		DrawText({110,27}, "your title", SKY_BLUE);
	}

   virtual void FrameUpdate() 
   { 
	   
	   	_DrawBox();

		DrawLine({vx1, vy1}, {vx2, vy2}, PURPLE);
	   	DrawLine({x,y}, {cos(angle) * 25 + x, sin(angle) * 25 + y}, ORANGE );	 
		DrawLine({px,py}, {cos(angle)*5 + px, sin(angle)*5 + py}, YELLOW);
	   	DrawCircle({x,y}, 4, SKY_BLUE, true);


		float tx1 = vx1 - px; float  ty1 = vy1 - py;
		float tx2 = vx2 - px; float  ty2 = vy2 - py;

		float tz1 = tx1 * cos(angle) + ty1 * sin(angle);
		float tz2 = tx2 * cos(angle) + ty2 * sin(angle);
		tx1 = tx1 * sin(angle) - ty1 * cos(angle);
		tx2 = tx2 * sin(angle) - ty2 * cos(angle);

		DrawLine({50-tx1, 50-tz1}, {50-tx2, 50-tz2}, PURPLE);
		DrawLine({50-tx1, 50-tz1}, {50-tx2, 100-tz2}, PURPLE);
		DrawLine({50,50},{50,45},PURPLE);
		if(tz1 > 0 || tz2 > 0) {

		float x1 = -tx1 * 16 / tz1; float y1a = -px / tz1; float y1b =  py / tz1;
		float x2 = -tx2 * 16 / tz2; float y2a = -px / tz2; float y2b =  py / tz2;

		DrawTextFormat({200,200}, "x1: %0.2f x2: %0.2f tx1: %0.2f tx2: %0.2f", x1, x2, tx1, tx2);		
		DrawLine({50+x1,50+y1a},{50+x2,50+y2a},YELLOW);
		DrawLine({50+x1,50+y1b},{50+x2,50+y2b},YELLOW);		
		DrawLine({50+x1,50+y1a},{50+x1,50+y1b}, SKY_BLUE);
		DrawLine({50+x2,50+y2a},{50+x2,50+y2b}, SKY_BLUE);
 		 }
   }

   virtual void KeyAPressed() {
	   ClearScreenRDP();
	   
   }

	virtual void KeyDUPressed() {
		px=px + sin(angle); py = py + cos(angle); 
		ClearScreenRDP();
	}

	virtual void KeyDDPressed() {
		px = px - sin(angle); py = py - cos(angle);
		ClearScreenRDP();
	}

   virtual void KeyJoyYPressed(int direction) 
   {
	   switch(direction) 
	   {
		   case Joystick::JoyUp  : px=px + cos(angle); py = py + sin(angle);  ClearScreenRDP();break;
		   case Joystick::JoyDown: px=px - cos(angle); py = py - sin(angle);  ClearScreenRDP();break;
	   }
   }
   virtual void KeyJoyXPressed(int direction) 
   {
	   switch(direction) 
	   {
		   case Joystick::JoyLeft : angle -= 0.01;  ClearScreenRDP(); break;
		   case Joystick::JoyRight: angle += 0.01;  ClearScreenRDP(); break;
	   }
   }
*/
	enum class ShapeTypes {SQUARE, TRIANGLE};
	
	struct PlacedShape 
	{
		bool MarkedForDeletion = false;
		ShapeTypes type;
		LibPos 	   pos;
		PlacedShape(LibPos p, ShapeTypes t) : pos(p), type(t) {};
	};

	std::list<PlacedShape> shapes;

private:
	int across = 16;
	LibPos shapePos = { 105, 78};
	ShapeTypes selection = ShapeTypes::SQUARE;
	
	void DrawGrid() 
	{
		for(int i =5;i<across;i++) 
		{
			DrawLine({ScreenWidth()/across * i, 0}, {ScreenWidth()/across * i, ScreenHeight()}, RED);
			DrawLine({0, ScreenHeight()/across * i}, {ScreenWidth(), ScreenHeight()/across * i}, SKY_BLUE);
		}
	}

	void DrawMenu() 
	{
		DrawTextCF({10,20}, "SELECTED");
		DrawTextFormatCF({10,40},"Placed %u", shapes.size());
		DrawTextFormatCF({10,30},"%s", (selection == ShapeTypes::TRIANGLE) ? "Triangle" : "Square");
	}

	void DrawShapes() {
		DrawRect(shapePos, {10,10}, GREY);

		if(!shapes.empty()) {
			for(auto s = shapes.begin(); s != shapes.end(); ++s) {
				switch(s->type) {
					case ShapeTypes::SQUARE  : DrawRect(s->pos, {10,10}, ORANGE); 										  break; 
					case ShapeTypes::TRIANGLE: DrawTriangle({s->pos.x, s->pos.y}, {s->pos.x, s->pos.y+10}, {s->pos.x + 10, s->pos.y}); break;
					default: break;
				}
			}
		}
	}
protected:
	sprite_t *font = nullptr;

	void FrameUpdate() override 
	{ 

		DrawGrid();
		DrawMenu();
		DrawShapes();
	}

	void OnCreate() override 
	{
		LoadCustomFont("/font.sprite");
		SetKeyState(KeyState::KeysDown);
	}

	virtual void KeyZPressed() override {
		ClearScreenRDP();
		selection = (selection == ShapeTypes::SQUARE) ? ShapeTypes::TRIANGLE : ShapeTypes::SQUARE;
	}

	virtual void KeyAPressed() override 
	{
		switch(selection) {
			case ShapeTypes::SQUARE  : shapes.emplace_back(PlacedShape(shapePos, ShapeTypes::SQUARE)); break;
			case ShapeTypes::TRIANGLE: shapes.emplace_back(PlacedShape(shapePos, ShapeTypes::TRIANGLE)); break;
		}
	}

	virtual void KeyBPressed() override {
		auto findShape = [&]() {
			for(auto s = shapes.begin(); s != shapes.end(); ++s) {
				if(s->pos == shapePos) {
					shapes.erase(s);
					return;

					DrawTextCF({40,40}, "Removed", YELLOW);
				}
			}
		};

		findShape();
	}

	virtual void KeyJoyXPressed(int direction) override 
	{
		switch(direction) 
	   {
		   case JoyLeft: {
			   shapePos.x-=20;
			ClearScreenRDP();
				} break;
		   case JoyRight: {
			   shapePos.x+=20;
			ClearScreenRDP();
			} break;
			default: break;
		}
	}

	virtual void KeyJoyYPressed(int direction) override
	{
	 	switch(direction) 
	   {
		   case Joystick::JoyUp : 
		   {
			   	shapePos.y-=15;
			   	ClearScreenRDP();
	
		   } break;
		   case Joystick::JoyDown: 
		   {
			    shapePos.y+=15;
			   	ClearScreenRDP();
		
		   } break;
	   }
	}



};

int main(void) {
	CPPTest n64g(RESOLUTION_320x240, DEPTH_32_BPP, ANTIALIAS_RESAMPLE, LibN64::Frame::UIType::GUI);
	n64g.Begin();
	
	return 0;
}
