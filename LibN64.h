#pragma once

#pragma GCC diagnostic ignored "-Wwrite-strings"

#include <libdragon.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iostream>

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
		int      Size();
		template <class T> 
		void	 Read(T buf, unsigned size, unsigned offset = 0x0) ;
		template <class T>
		T        QuickRead(const char* file);
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

			virtual void KeyCUPressed();
			virtual void KeyCDPressed();
			virtual void KeyCLPressed();
			virtual void KeyCRPressed();
			
			virtual void KeyJoyXPressed(int);
			virtual void KeyJoyYPressed(int);

			inline virtual void     __OnInit_FreeFunction1(); 
			inline virtual void     __OnInit_FreeFunction2();
			inline virtual void     __OnLoop_FreeFunction1();
			inline virtual void     __OnLoop_FreeFunction2();

			unsigned screenWidth;
			unsigned screenHeight;

			void CheckAndSwitchRes(resolution_t r);
		public:
			display_context_t d;

			std::string GameTitle = "Default";
			bool lActive;

			float fFrameTime;
			float fTotalTime;

			int uitype;

			sprite_t* libFont = nullptr;
		public:
			struct   LibPos        { int x, y; };
			enum     UIType        { GUI, CONSOLE };
			enum     Joystick      { JoyUp=0x00000072, JoyDown=0x0000008E, JoyLeft=0x00008E00, JoyRight=0x00007200 };
			enum 	 KeyState 	   { KeysHeld, KeysDown, KeysPressed, KeysUp } kstate;
			Frame(resolution_t res, bitdepth_t dep, antialias_t aa, UIType ui);
			virtual void OnCreate  ();
			void 	 	 Begin	       ();
			void 	 	 Close		   ();
			void 	 	 ClearScreen   ();
			void 	 	 ClearScreenRDP();
			void 	 	 SetScreen     (resolution_t res, bitdepth_t bd); 
	display_context_t    GetDisplay    ();
			void		 SetKeyState   (KeyState k);
			unsigned 	 ScreenWidth   ();
			unsigned 	 ScreenHeight  ();
			void 	     DrawTextFormat(LibPos pos, const std::string format, ...);
			void 	 	 DrawText      (LibPos pos, const std::string t,    		unsigned c = WHITE);
	 		void 		 DrawPixel 	   (LibPos pos, 		      	        		unsigned c = WHITE);
			void 		 DrawRect      (LibPos pos, LibPos dimensions={1,1},		unsigned c = WHITE);
			void     	 DrawRDPRect   (LibPos pos, LibPos dimensions={1,1},		unsigned c = WHITE);
	 		void 		 DrawLine  	   (LibPos pos1, LibPos pos2,           		unsigned c = WHITE);
			void  	 	 DrawCircle    (LibPos pos, int scale=1,float cStepSize=0.1,unsigned c = WHITE, bool isFilled = true);
			void	 	 DrawTriangle  (LibPos pos1,LibPos pos2,LibPos pos3,		unsigned c = WHITE); 
			void     	 DrawRDPTri    (LibPos pos1,LibPos pos2,LibPos pos3,		unsigned c = WHITE);
			void     	 DrawSprite    (LibPos, sprite_t* spr);
			float    	 Ticks2Seconds (float t); 

			void	 	 LoadCustomFont  (const std::string FileName);
			void 		 DrawTextCF      (LibPos pos, const std::string str);
			void   		 DrawTextFormatCF(LibPos pos, const std::string format, ...);

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
		};
}
