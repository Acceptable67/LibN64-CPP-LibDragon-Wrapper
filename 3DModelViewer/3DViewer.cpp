#include <stdio.h>
#include <sstream>
#include "3DViewer.h"

#include <memory>
#include <sstream>
#include <typeinfo>
#include <bitset>
#include <algorithm>
#include <list>
#include <ctime>

void ModelViewer::KeyAPressed() {
	
		vec3d vForward = Vector_Mul(vLookDir, 8.0f * fFrameTime);
		vCamera = Vector_Add(vCamera, vForward);
}
void ModelViewer::KeyBPressed() {
	
		vec3d vForward = Vector_Mul(vLookDir, 8.0f * GetFrameTime());
    	vCamera = Vector_Sub(vCamera, vForward);
}

void ModelViewer::KeyZPressed() {
	
		vec3d vForward = Vector_Mul(vLookDir, 8.0f * GetFrameTime());
		fYaw -= 2.0f * GetFrameTime();
	
}

void ModelViewer::KeyDUPressed() {
	vCamera.y += 8.0f * GetFrameTime();	// Travel Upwards
	
}
void ModelViewer::KeyDDPressed() {
			vCamera.y -= 8.0f * GetFrameTime();	// Travel Downwards

}
void ModelViewer::KeyDLPressed() {
			vCamera.x -= 8.0f * GetFrameTime();	// Travel Along X-Axis
		
}
void ModelViewer::KeyDRPressed() {

			vCamera.x += 8.0f * GetFrameTime();	// Travel Along X-Axis
}
struct Box {
   public:
      Frame::LibPos coords;
      int width, height;

      Box(int _x, int _y, int w, int h) : coords({_x,_y}), width(w), height(h) {};

      inline void Draw(LibN64::Frame& r) {
       		r.DrawRect(coords, {width, height},  WHITE);
      }
};

void ModelViewer::FrameUpdate()
{

		// makes this a bit redundant
		mat4x4 matRotZ, matRotX;
		//fTheta += 1.0f * GetFrameTime(); // Uncomment to spin me right round baby right round
		matRotZ = Matrix_MakeRotationZ(fTheta * 0.5f);
		matRotX = Matrix_MakeRotationX(fTheta);

		mat4x4 matTrans;
		matTrans = Matrix_MakeTranslation(0.0f, 0.0f, 5.0f);

		mat4x4 matWorld;
		matWorld = Matrix_MakeIdentity();	// Form World Matrix
		matWorld = Matrix_MultiplyMatrix(matRotZ, matRotX); // Transform by rotation
		matWorld = Matrix_MultiplyMatrix(matWorld, matTrans); // Transform by translation

		// Create "Point At" Matrix for camera
		vec3d vUp = { 0,1,0 };
		vec3d vTarget = { 0,0,1 };
		mat4x4 matCameraRot = Matrix_MakeRotationY(fYaw);
		vLookDir = Matrix_MultiplyVector(matCameraRot, vTarget);
		vTarget = Vector_Add(vCamera, vLookDir);
		mat4x4 matCamera = Matrix_PointAt(vCamera, vTarget, vUp);

		// Make view matrix from camera
		mat4x4 matView = Matrix_QuickInverse(matCamera);

		// Store triagles for rastering later
		vector<triangle> vecTrianglesToRaster;

		// Draw Triangles
		for (auto tri : meshCube.tris)
		{
			triangle triProjected, triTransformed, triViewed;

			// World Matrix Transform
			triTransformed.p[0] = Matrix_MultiplyVector(matWorld, tri.p[0]);
			triTransformed.p[1] = Matrix_MultiplyVector(matWorld, tri.p[1]);
			triTransformed.p[2] = Matrix_MultiplyVector(matWorld, tri.p[2]);

			vec3d normal, line1, line2;

			line1  = Vector_Sub(triTransformed.p[1], triTransformed.p[0]);
			line2  = Vector_Sub(triTransformed.p[2], triTransformed.p[0]);
			normal = Vector_CrossProduct(line1, line2);
			normal = Vector_Normalise(normal);
	   
			vec3d vCameraRay = Vector_Sub(triTransformed.p[0], vCamera);

			// If ray is aligned with normal, then triangle is visible
			if (Vector_DotProduct(normal, vCameraRay) < 0.0f)
			{
				// Illumination
				vec3d light_direction = { 0.0f, 1.0f, -1.0f };
				light_direction = Vector_Normalise(light_direction);

				float dp = max(0.1f, Vector_DotProduct(light_direction, normal));

				triViewed.p[0] = Matrix_MultiplyVector(matView, triTransformed.p[0]);
				triViewed.p[1] = Matrix_MultiplyVector(matView, triTransformed.p[1]);
				triViewed.p[2] = Matrix_MultiplyVector(matView, triTransformed.p[2]);

				// Clip Viewed Triangle against near plane, this could form two additional
				// additional triangles. 
				int nClippedTriangles = 0;
				triangle clipped[2];
				nClippedTriangles = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

				for (int n = 0; n < nClippedTriangles; n++)
				{
					// Project triangles from 3D --> 2D
					triProjected.p[0] = Matrix_MultiplyVector(matProj, clipped[n].p[0]);
					triProjected.p[1] = Matrix_MultiplyVector(matProj, clipped[n].p[1]);
					triProjected.p[2] = Matrix_MultiplyVector(matProj, clipped[n].p[2]);
	
					triProjected.p[0] = Vector_Div(triProjected.p[0], triProjected.p[0].w);
					triProjected.p[1] = Vector_Div(triProjected.p[1], triProjected.p[1].w);
					triProjected.p[2] = Vector_Div(triProjected.p[2], triProjected.p[2].w);

					triProjected.p[0].x *= -1.0f;
					triProjected.p[1].x *= -1.0f;
					triProjected.p[2].x *= -1.0f;
					triProjected.p[0].y *= -1.0f;
					triProjected.p[1].y *= -1.0f;
					triProjected.p[2].y *= -1.0f;

					vec3d vOffsetView = { 1,1,0 };
					triProjected.p[0] = Vector_Add(triProjected.p[0], vOffsetView);
					triProjected.p[1] = Vector_Add(triProjected.p[1], vOffsetView);
					triProjected.p[2] = Vector_Add(triProjected.p[2], vOffsetView);
					triProjected.p[0].x *= 0.5f * (float)ScreenWidth();
					triProjected.p[0].y *= 0.5f * (float)ScreenHeight();
					triProjected.p[1].x *= 0.5f * (float)ScreenWidth();
					triProjected.p[1].y *= 0.5f * (float)ScreenHeight();
					triProjected.p[2].x *= 0.5f * (float)ScreenWidth();
					triProjected.p[2].y *= 0.5f * (float)ScreenHeight();

					// Store triangle for sorting
					vecTrianglesToRaster.push_back(triProjected);
				}			
			}
		}

		sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle &t1, triangle &t2)
		{
			float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
			float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
			return z1 > z2;
		});

		for (auto &triToRaster : vecTrianglesToRaster)
		{
			triangle clipped[2];
			list<triangle> listTriangles;

			// Add initial triangle
			listTriangles.push_back(triToRaster);
			int nNewTriangles = 1;

			for (int p = 0; p < 4; p++)
			{
				int nTrisToAdd = 0;
				while (nNewTriangles > 0)
				{
					// Take triangle from front of queue
					triangle test = listTriangles.front();
					listTriangles.pop_front();
					nNewTriangles--;

					switch (p)
					{
					case 0:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 1:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, (float)ScreenHeight() - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 2:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 3:	nTrisToAdd = Triangle_ClipAgainstPlane({ (float)ScreenWidth() - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					}

					for (int w = 0; w < nTrisToAdd; w++)
						listTriangles.push_back(clipped[w]);
				}
				nNewTriangles = listTriangles.size();
			}


			for (auto &t : listTriangles)
			{
				DrawTriangle(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y);
			}
		}


}

auto ModelViewer::__OnLoop_FreeFunction1() -> void 
{
		if(GetFrameTime() >= 0.07) {
			ClearScreenRDP();
		}
}

auto ModelViewer::OnCreate() -> void
{
		ClearScreenRDP();
	    debug_init(DEBUG_FEATURE_ALL);
		meshCube.LoadFromObjectFile(*this, "/VideoShip.obj");

		matProj = Matrix_MakeProjection(90.0f, (float)ScreenHeight() / (float)ScreenWidth(), 0.1f, 1000.0f);

}


int main(void) {
	ModelViewer n64g(RESOLUTION_320x240, DEPTH_16_BPP, ANTIALIAS_RESAMPLE, Frame::UIType::GUI);
	n64g.Begin();
	return 0;
}
