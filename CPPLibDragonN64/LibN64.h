#pragma once
#include <libdragon.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#define GUI	    1
#define CONSOLE 2

#define RED				graphics_make_color(0xFF, 0x00, 0x00, 0xFF)
#define GREEN			graphics_make_color(0x00, 0xFF, 0x00, 0xFF)
#define WHITE			graphics_make_color(0xFF, 0xFF, 0xFF, 0xFF)
#define NAVY_BLUE		graphics_make_color(17, 24, 128,255)
#define DARK_RED		graphics_make_color(75, 19, 0,255)
#define DEEP_DARK_RED	graphics_make_color(31, 1, 0,255)
#define SKY_BLUE		graphics_make_color(17, 24, 215,255)
#define ORANGE			graphics_make_color(0xFF, 0xA5, 0x00, 0xFF)
#define GOLD			graphics_make_color(0xFF, 0xD7, 0x00, 255)
#define YELLOW			graphics_make_color(0xFF, 0xFF, 0x00, 255)
#define DARK_GREEN		graphics_make_color(0x00, 0x80, 0x00, 255)
#define CYAN			graphics_make_color(0x00, 0xFF, 0xFF, 255)
#define GREY			graphics_make_color(0x80, 0x80, 0x80, 255)
#define PURPLE			graphics_make_color(0xFF, 0x00, 0x9B, 0xFF)

#define TICKS_TOTAL(since_start)	(timer_ticks()-since_start) * 0.021333333 / 1000000.0
#define TICKS2SECONDS(ticks)		(ticks * 0.021333333 / 1000000.0)

typedef char byte;

namespace LibN64 
{
	namespace DFS 
	{
		unsigned Open(const char* file) ;
		int      Size(const char* file);
		void	 Read(char* buf, unsigned size, unsigned offset = 0x0) ;
		char*    QuickRead(const char* file);
		void     Close();
	};

	class Frame 
	{
		private:
			virtual void FrameUpdate();
			virtual void KeyAPressed();
			virtual void KeyBPressed();
			virtual void KeyZPressed();

			virtual void KeyStartPressed();

			virtual void KeyDUPressed();
			virtual void KeyDDPressed();
			virtual void KeyDLPressed();
			virtual void KeyDRPressed();
			
			virtual void KeyJoyXPressed();
			virtual void KeyJoyYPressed();

			unsigned screenWidth;
			unsigned screenHeight;

			void CheckAndSwitchRes(resolution_t r);
		public:
			display_context_t d;

			const char* romTitle = "Default";
			bool lActive;

			float fFrameTime;
			float fTotalTime;

			int       uitype;
		public:
			Frame(resolution_t res, bitdepth_t dep, int ui);
			virtual void OnCreate();
			void 	 Begin();
			void 	 Close();
			void 	 ClearScreen();
			void 	 SetScreen(resolution_t res, bitdepth_t bd);
			unsigned ScreenWidth();
			unsigned ScreenHeight();
			void 	 DrawText(int x, int y, const char* t,    unsigned c = WHITE);
			void 	 DrawPixel(int x, int y, 		      	  unsigned c = WHITE);
			void 	 DrawBox(int x, int y, int scale = 1,     unsigned c = WHITE);
			void 	 DrawLine(int x1, int y1, int x2, int y2, unsigned c = WHITE);
			void  	 DrawCircle(int x, int y, int scale = 1,  unsigned c = WHITE);
			float Ticks2Seconds(float t);

			/*The following functions refuse to compile inside the C++ file.*/
			/*DFS does not work so here is work around. Manually find*/
		public:
			template<class T>
			T* __lib64_rom2buf(long romAddr, int size) {
				T* tmp = (T*)malloc(size + sizeof(T));
				for (auto i = 0; i < size; i++) {
					tmp[i] = __lib64_rom2type<T>(romAddr + (i*sizeof(T)));
				}
				return tmp;
			}

			template<class T>
			T __lib64_rom2type(long romAddr) {
				T *ptr = (T*)(romAddr);
				return *ptr;
			}

			void DrawTextFormat(int x, int y, const char* format, ...) {
			va_list args;
			va_start(args, format);
			char buffer[125];
			vsprintf(buffer, format, args);
			graphics_draw_text(d, x, y,buffer);
			va_end(args);	
			}
		};
}
