#include <stdio.h>
#include <sstream>
#include "CPPTesting.h"

#include <memory>
#include <typeinfo>
#include <bitset>
#include <algorithm>
#include <chrono>
#include <ctime>

//char *p = (char*)'c';
std::shared_ptr<char> p = std::make_shared<char>('c');

void incNum(CPPTest& r) {
	r.privvar = 2;
}

Frame::LibPos cirPos = {10,40};
long start, end;

volatile double t2 = 0;

float num = 0;
int lx = 40, ly = 10, activeshape = 0;

int *shapecoords[2][2] = 
{ 
	{&cirPos.x, &cirPos.y},
	{&lx, &ly}
};

void CPPTest::KeyAPressed() {
	nMode = 0;
}
void CPPTest::KeyBPressed() {
    nMode = 1;
}

sprite_t *t, *b;
float fElapsedTime = 0.590009;
void CPPTest::clearscr() {
	rdp_sync ( SYNC_PIPE );
		rdp_set_default_clipping();
		rdp_set_primitive_color(0x0);
		rdp_set_blend_color(WHITE);
		rdp_enable_texture_copy();
		rdp_enable_primitive_fill();
		rdp_attach_display(this->d);
	rdp_draw_filled_rectangle(0,0,320,240);

	rdp_sync(SYNC_PIPE);
	
}

void CPPTest::KeyZPressed() {
	nMode = 2;
	
}

void CPPTest::KeyDUPressed() {
			vecShapes[0].pos.x += cosf(vecShapes[0].angle) * 60.0f * fElapsedTime;
			vecShapes[0].pos.y += sinf(vecShapes[0].angle) * 60.0f * fElapsedTime;
	
}
void CPPTest::KeyDDPressed() {
				vecShapes[0].pos.x -= cosf(vecShapes[0].angle) * 60.0f * fElapsedTime;
			vecShapes[0].pos.y -= sinf(vecShapes[0].angle) * 60.0f * fElapsedTime;
}
void CPPTest::KeyDLPressed() {
vecShapes[0].angle -= 2.0f * fElapsedTime;
		
}
void CPPTest::KeyDRPressed() {
vecShapes[0].angle += 2.0f * fElapsedTime;
	clearscr();
}
struct Box {
   public:
      Frame::LibPos coords;
      int width, height;

      Box(int _x, int _y, int w, int h) : coords({_x,_y}), width(w), height(h) {};

      inline void Draw(LibN64::Frame& r) {
       		graphics_draw_box(r.d, coords.x,coords.y, width, height, WHITE);
      }
};

bool doOverlap(Frame::LibPos l1, Frame::LibPos r1, Frame::LibPos l2, Frame::LibPos r2)
{
	//graphics_fill_screen(this->d,0x0);
    if (l1.x == r1.x || l1.y == r1.y || l2.x == r2.x || l2.y == r2.y) {
        return false;
    }
    if (l1.x >= r2.x || l2.x >= r1.x) return false;
    if (r1.y >= l2.y || r2.y >= l1.y) return false;
 
    return true;
}

Box sq(20,80,200,100);

inline void CPPTest::DrawingArea() {
	sq.Draw(*this);
}

double ctr = 0;
void half_sec(int ovfl) {
	ctr += 1;
}

void CPPTest::FrameUpdate()
{
	// Update Shapes and reset flags
		for (auto &r : vecShapes)
		{
			for (int i = 0; i < r.o.size(); i++)
				r.p[i] =
				{	// 2D Rotation Transform + 2D Translation
					(r.o[i].x * cosf(r.angle)) - (r.o[i].y * sinf(r.angle)) + r.pos.x,
					(r.o[i].x * sinf(r.angle)) + (r.o[i].y * cosf(r.angle)) + r.pos.y,
				};

			r.overlap = false;
		}

		// Check for overlap
		for (int m = 0; m < vecShapes.size(); m++)
			for (int n = m + 1; n < vecShapes.size(); n++)
			{
				switch (nMode)
				{
				case 0: vecShapes[m].overlap |= ShapeOverlap_SAT(vecShapes[m], vecShapes[n]); break;
				case 1: vecShapes[m].overlap |= ShapeOverlap_SAT_STATIC(vecShapes[m], vecShapes[n]); break;
				case 2: vecShapes[m].overlap |= ShapeOverlap_DIAGS(vecShapes[m], vecShapes[n]); break;
				case 3: vecShapes[m].overlap |= ShapeOverlap_DIAGS_STATIC(vecShapes[m], vecShapes[n]); break;
				}						
			}
		
		// === Render Di	
		rdp_sync ( SYNC_PIPE );
		rdp_set_default_clipping();
		rdp_set_primitive_color(0x0);
		rdp_set_blend_color(0x0);
		rdp_enable_texture_copy();
		rdp_enable_primitive_fill();
		rdp_attach_display(this->d);
	rdp_draw_filled_rectangle(0,0,320,240);

	rdp_sync(SYNC_PIPE);

		// Draw Shapes
		for (auto &r : vecShapes)
		{
			// Draw Boundary
			for (int i = 0; i < r.p.size(); i++)
				DrawLine({(int)r.p[i].x, (int)r.p[i].y}, {(int)r.p[(i + 1) % r.p.size()].x, (int)r.p[(i + 1) % r.p.size()].y}, (r.overlap ? RED : WHITE));

			// Draw Direction
			DrawLine({(int)r.p[0].x, (int)r.p[0].y}, {(int)r.pos.x, (int)r.pos.y}, (r.overlap ? RED : WHITE));
		}

}

auto CPPTest::__OnLoop_FreeFunction1() -> void 
{
	
}

auto CPPTest::__OnInit_FreeFunction1() -> void
{
	
}

auto CPPTest::OnCreate() -> void
{
// Create Pentagon
		polygon s1;
		float fTheta = 3.14159f * 2.0f / 5.0f;
		s1.pos = { 100, 100 };
		s1.angle = 0.0f;
		for (int i = 0; i < 5; i++)
		{
			s1.o.push_back({ 30.0f * cosf(fTheta * i), 30.0f * sinf(fTheta * i) });
			s1.p.push_back({ 30.0f * cosf(fTheta * i), 30.0f * sinf(fTheta * i) });
		}

		// Create Triangle
		polygon s2;
		fTheta = 3.14159f * 2.0f / 3.0f;
		s2.pos = { 200, 150 };
		s2.angle = 0.0f;
		for (int i = 0; i < 3; i++)
		{
			s2.o.push_back({ 20.0f * cosf(fTheta * i), 20.0f * sinf(fTheta * i) });
			s2.p.push_back({ 20.0f * cosf(fTheta * i), 20.0f * sinf(fTheta * i) });
		}

		// Create Quad
		polygon s3;
		s3.pos = { 50, 200 };
		s3.angle = 0.0f;
		s3.o.push_back({ -30, -30 });
		s3.o.push_back({ -30, +30 });
		s3.o.push_back({ +30, +30 });
		s3.o.push_back({ +30, -30 });
		s3.p.resize(4);
		
			
		vecShapes.push_back(s1);
		vecShapes.push_back(s2);
		vecShapes.push_back(s3);



}


int main(void) {
	CPPTest n64g(RESOLUTION_320x240, DEPTH_16_BPP, GUI);
	n64g.Begin();
	return 0;
}
