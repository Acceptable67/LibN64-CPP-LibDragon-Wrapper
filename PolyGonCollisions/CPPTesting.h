
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <map>
#include <set>
#include <stack>

#include <functional>

#include "LibN64.h"

using namespace LibN64;

class CPPTest : public Frame  {
private:	
	
	struct vec2d
	{
		float x;
		float y;
	};

	struct polygon
	{
		std::vector<vec2d> p;	// Transformed Points
		vec2d pos;				// Position of shape
		float angle;			// Direction of shape
		std::vector<vec2d> o;	// "Model" of shape							
		bool overlap = false;	// Flag to indicate if overlap has occurred
	};

	std::vector<polygon> vecShapes;

	int nMode = 0;


	int privvar;
 public:
   CPPTest(resolution_t res, bitdepth_t dep, int ui) : Frame(res, dep, ui) {}

bool ShapeOverlap_SAT(polygon &r1, polygon &r2)
	{
		polygon *poly1 = &r1;
		polygon *poly2 = &r2;

		for (int shape = 0; shape < 2; shape++)
		{
			if (shape == 1)
			{
				poly1 = &r2;
				poly2 = &r1;
			}
		
			for (int a = 0; a < poly1->p.size(); a++)
			{
				int b = (a + 1) % poly1->p.size();
				vec2d axisProj = { -(poly1->p[b].y - poly1->p[a].y), poly1->p[b].x - poly1->p[a].x };
				float d = sqrtf(axisProj.x * axisProj.x + axisProj.y * axisProj.y);
				axisProj = { axisProj.x / d, axisProj.y / d };

				// Work out min and max 1D points for r1
				float min_r1 = INFINITY, max_r1 = -INFINITY;
				for (int p = 0; p < poly1->p.size(); p++)
				{
					float q = (poly1->p[p].x * axisProj.x + poly1->p[p].y * axisProj.y);
					min_r1 = std::min(min_r1, q);
					max_r1 = std::max(max_r1, q);
				}

				// Work out min and max 1D points for r2
				float min_r2 = INFINITY, max_r2 = -INFINITY;
				for (int p = 0; p < poly2->p.size(); p++)
				{
					float q = (poly2->p[p].x * axisProj.x + poly2->p[p].y * axisProj.y);
					min_r2 = std::min(min_r2, q);
					max_r2 = std::max(max_r2, q);
				}

				if (!(max_r2 >= min_r1 && max_r1 >= min_r2))
					return false;
			}
		}

		return true;
	}

	bool ShapeOverlap_SAT_STATIC(polygon &r1, polygon &r2)
	{
		polygon *poly1 = &r1;
		polygon *poly2 = &r2;

		float overlap = INFINITY;
		
		for (int shape = 0; shape < 2; shape++)
		{
			if (shape == 1)
			{
				poly1 = &r2;
				poly2 = &r1;
			}

			for (int a = 0; a < poly1->p.size(); a++)
			{
				int b = (a + 1) % poly1->p.size();
				vec2d axisProj = { -(poly1->p[b].y - poly1->p[a].y), poly1->p[b].x - poly1->p[a].x };
				
				// Optional normalisation of projection axis enhances stability slightly
				//float d = sqrtf(axisProj.x * axisProj.x + axisProj.y * axisProj.y);
				//axisProj = { axisProj.x / d, axisProj.y / d };

				// Work out min and max 1D points for r1
				float min_r1 = INFINITY, max_r1 = -INFINITY;
				for (int p = 0; p < poly1->p.size(); p++)
				{
					float q = (poly1->p[p].x * axisProj.x + poly1->p[p].y * axisProj.y);
					min_r1 = std::min(min_r1, q);
					max_r1 = std::max(max_r1, q);
				}

				// Work out min and max 1D points for r2
				float min_r2 = INFINITY, max_r2 = -INFINITY;
				for (int p = 0; p < poly2->p.size(); p++)
				{
					float q = (poly2->p[p].x * axisProj.x + poly2->p[p].y * axisProj.y);
					min_r2 = std::min(min_r2, q);
					max_r2 = std::max(max_r2, q);
				}

				// Calculate actual overlap along projected axis, and store the minimum
				overlap = std::min(std::min(max_r1, max_r2) - std::max(min_r1, min_r2), overlap);

				if (!(max_r2 >= min_r1 && max_r1 >= min_r2))
					return false;
			}
		}

		// If we got here, the objects have collided, we will displace r1
		// by overlap along the vector between the two object centers
		vec2d d = { r2.pos.x - r1.pos.x, r2.pos.y - r1.pos.y };
		float s = sqrtf(d.x*d.x + d.y*d.y);
		r1.pos.x -= overlap * d.x / s;
		r1.pos.y -= overlap * d.y / s;
		return false;
	}

	// Use edge/diagonal intersections.
	bool ShapeOverlap_DIAGS(polygon &r1, polygon &r2)
	{
		polygon *poly1 = &r1;
		polygon *poly2 = &r2;

		for (int shape = 0; shape < 2; shape++)
		{
			if (shape == 1)
			{
				poly1 = &r2;
				poly2 = &r1;
			}
			
			// Check diagonals of polygon...
			for (int p = 0; p < poly1->p.size(); p++)
			{
				vec2d line_r1s = poly1->pos;
				vec2d line_r1e = poly1->p[p];

				// ...against edges of the other
				for (int q = 0; q < poly2->p.size(); q++)
				{
					vec2d line_r2s = poly2->p[q];
					vec2d line_r2e = poly2->p[(q + 1) % poly2->p.size()];

					// Standard "off the shelf" line segment intersection
					float h = (line_r2e.x - line_r2s.x) * (line_r1s.y - line_r1e.y) - (line_r1s.x - line_r1e.x) * (line_r2e.y - line_r2s.y);
					float t1 = ((line_r2s.y - line_r2e.y) * (line_r1s.x - line_r2s.x) + (line_r2e.x - line_r2s.x) * (line_r1s.y - line_r2s.y)) / h;
					float t2 = ((line_r1s.y - line_r1e.y) * (line_r1s.x - line_r2s.x) + (line_r1e.x - line_r1s.x) * (line_r1s.y - line_r2s.y)) / h;

					if (t1 >= 0.0f && t1 < 1.0f && t2 >= 0.0f && t2 < 1.0f)
					{
						return true;
					}
				}
			}		
		}
		return false;
	}

	// Use edge/diagonal intersections.
	bool ShapeOverlap_DIAGS_STATIC(polygon &r1, polygon &r2)
	{
		polygon *poly1 = &r1;
		polygon *poly2 = &r2;

		for (int shape = 0; shape < 2; shape++)
		{
			if (shape == 1)
			{
				poly1 = &r2;
				poly2 = &r1;
			}

			// Check diagonals of this polygon...
			for (int p = 0; p < poly1->p.size(); p++)
			{
				vec2d line_r1s = poly1->pos;
				vec2d line_r1e = poly1->p[p];

				vec2d displacement = { 0,0 };

				// ...against edges of this polygon
				for (int q = 0; q < poly2->p.size(); q++)
				{
					vec2d line_r2s = poly2->p[q];
					vec2d line_r2e = poly2->p[(q + 1) % poly2->p.size()];

					// Standard "off the shelf" line segment intersection
					float h = (line_r2e.x - line_r2s.x) * (line_r1s.y - line_r1e.y) - (line_r1s.x - line_r1e.x) * (line_r2e.y - line_r2s.y);
					float t1 = ((line_r2s.y - line_r2e.y) * (line_r1s.x - line_r2s.x) + (line_r2e.x - line_r2s.x) * (line_r1s.y - line_r2s.y)) / h;
					float t2 = ((line_r1s.y - line_r1e.y) * (line_r1s.x - line_r2s.x) + (line_r1e.x - line_r1s.x) * (line_r1s.y - line_r2s.y)) / h;

					if (t1 >= 0.0f && t1 < 1.0f && t2 >= 0.0f && t2 < 1.0f)
					{						
						displacement.x += (1.0f - t1) * (line_r1e.x - line_r1s.x);
						displacement.y += (1.0f - t1) * (line_r1e.y - line_r1s.y);					
					}
				}

				r1.pos.x += displacement.x * (shape == 0 ? -1 : +1);
				r1.pos.y += displacement.y * (shape == 0 ? -1 : +1);
			}
		}

		// Cant overlap if static collision is resolved
		return false;
	}

   void DrawingArea();
   virtual void OnCreate() override;
   virtual void FrameUpdate() override;
   void clearscr();

   virtual void KeyDUPressed() override;
   virtual void KeyDDPressed() override;
   virtual void KeyDLPressed() override;
   virtual void KeyDRPressed() override;
   virtual void KeyZPressed() override;

   virtual void KeyAPressed() override;
   virtual void KeyBPressed() override;
    virtual void KeyZPressed() override;

     //    virtual void KeyJoyXPressed() override;
     // virtual void KeyJoyYPressed() override;
   virtual void __OnLoop_FreeFunction1() override;
   virtual void __OnInit_FreeFunction1() override;

   std::function<void()> wrapper;
   friend void incNum(CPPTest& r);
};