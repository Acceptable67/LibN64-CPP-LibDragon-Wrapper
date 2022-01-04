#pragma once
#pragma GCC diagnostic ignored "-Wwrite-strings"

#include <libdragon.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <vector>
#include <map>
#include <iostream>

#define TICKS_TOTAL(since_start)	(timer_ticks()-since_start) * 0.021333333 / 1000000.0

typedef char byte;

class LibMenuManager;
class LibMenu;

constexpr int MakeColor(int r, int g, int b, int a) 
{
	return (r << 24) | (((g & 0x00FFFFFF) << 16)) | (((b & 0xFF00FFFF) << 8)) | ((a & 0xFFFF00FF));
}

namespace LibN64 
{
	enum Color 
	{
		 RED			= MakeColor(0xFF,0x00,0x00,0xFF),
		 GREEN			= MakeColor(0x00, 0xFF, 0x00, 0xFF),
		 WHITE			= MakeColor(0xFF, 0xFF, 0xFF, 0xFF),
		 BLACK          = MakeColor(0x00, 0x00, 0x00, 0xFF),
		 BLUE           = MakeColor(0x00, 0x00, 0xFF, 0xFF),
		 NAVY_BLUE		= MakeColor(17, 24, 128,255),
		 DARK_RED		= MakeColor(75, 19, 0,255),
		 DEEP_DARK_RED	= MakeColor(31, 1, 0,255),
		 SKY_BLUE		= MakeColor(17, 24, 215,255),
		 ORANGE			= MakeColor(0xFF, 0xA5, 0x00, 0xFF),
		 GOLD			= MakeColor(0xFF, 0xD7, 0x00, 255),
		 YELLOW			= MakeColor(0xFF, 0xFF, 0x00, 255),
		 DARK_GREEN		= MakeColor(0x00, 0x80, 0x00, 255),
		 CYAN			= MakeColor(0x00, 0xFF, 0xFF, 255),
		 GREY			= MakeColor(0x80, 0x80, 0x80, 255),
		 PURPLE			= MakeColor(0xFF, 0x00, 0x9B, 0xFF)
	};
	

	struct LibPos 
	{ 
		int x, y; 
		LibPos operator +(LibPos x) const 
		{
			LibPos tmp = { this->x, this->x};
			tmp.x += x.x;
			tmp.y += x.y;
			return tmp;
		}

		LibPos operator -(LibPos x) const 
		{
			LibPos tmp = { this->x, this->x};
			tmp.x -= x.x;
			tmp.y -= x.y;
			return tmp;
		}
				
		bool operator ==(LibPos x) const
		{
			LibPos tmp = { this->x, this->y };
			return (tmp.x == x.x && tmp.y == x.y) ? true : false;
		}
	};

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

		protected:
			display_context_t d;
			sprite_t* 		  libFont = nullptr;
			std::string 	  GameTitle = "Default";

			bool 	lActive;
			float 	fFrameTime;
			float 	fTotalTime;
			int     iFrames;
			float   fFrameRate;
			float   fFrameCounter;
			int 	uitype;
			bool    bDLInLoop = false;
			float   controllerScanRate = 0.02;
		
		public:
			enum     UIType        { GUI, CONSOLE };
			enum     Joystick      { JoyUp=0x00000072, JoyDown=0x0000008E, JoyLeft=0x00008E00, JoyRight=0x00007200 };
			enum 	 KeyState 	   { KeysHeld, KeysDown, KeysPressed, KeysUp } kstate;
			Frame(resolution_t res, bitdepth_t dep, antialias_t aa, UIType ui);
			virtual void OnCreate  ();
			void 	 	 Begin	       ();
			void 	 	 Close		   ();
			void 	 	 ClearScreen   ();
			void 	 	 ClearScreenRDP(unsigned c = BLACK);
			void		 SetControllerScanRate(float fRate);
			void 	 	 SetScreen     (resolution_t res, bitdepth_t bd); 
	display_context_t    GetDisplay    ();
			void		 SetKeyState   (KeyState k);
			void 		 SetDLInLoop   ();
			unsigned 	 ScreenWidth   ();
			unsigned 	 ScreenHeight  ();
			void 	     DrawTextFormat(LibPos pos, const std::string format, ...);
			void 	     DrawTextFormat(LibPos pos, Color forecolor, 		   		Color backcolor, const std::string format, ...);
			void 	 	 DrawText      (LibPos pos, const std::string t,    		Color forecolor = WHITE, Color backcolor = BLACK);
	 		void 		 DrawPixel 	   (LibPos pos, 		      	        		unsigned c = WHITE);
			void 		 DrawRect      (LibPos pos, LibPos dimensions={1,1},		unsigned c = WHITE, bool isFilled = true);
			void     	 DrawRDPRect   (LibPos pos, LibPos dimensions={1,1},		unsigned c = WHITE);
	 		void 		 DrawLine  	   (LibPos pos1, LibPos pos2,           		unsigned c = WHITE);
			void  	 	 DrawCircle    (LibPos pos, int scale=1,float cStepSize=0.1,unsigned c = WHITE, bool isFilled = true);
			void	 	 DrawTriangle  (LibPos pos1,LibPos pos2,LibPos pos3,		unsigned c = WHITE); 
			void     	 DrawRDPTri    (LibPos pos1,LibPos pos2,LibPos pos3,		unsigned c = WHITE);
			void     	 DrawSprite    (LibPos, sprite_t* spr);
			float    	 Ticks2Seconds (float t); 

			/*uses Baremetal-MIPS 8x8 font from PeterLemon (github)*/
			void	 	 LoadCustomFont  (const std::string FileName);
			void 		 DrawTextCF      (LibPos pos, const std::string str, int color = WHITE);
			void   		 DrawTextFormatCF(LibPos pos, const std::string format, ...);

			float        GetTotalTime();
			float        GetFrameTime();
			float        GetFrameRate();
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

			typedef int ID;
			class LibMenu 
			{
				public:
					ID          id;
					LibPos 		pos;
					LibPos 		size;
					std::string title;
					std::string content;
					Color 		forecolor, backcolor;
					bool 		inFocus;

					int         menuItemCount;
					int			menuItemSelection;
					std::map<int, std::string> menuItems;
						
				public:
					LibMenu(ID i, std::string t, LibPos p, LibPos s, Color f = BLACK, Color b = WHITE)
					{ 
						id = i; pos = p; size = s; title = t; forecolor = f; backcolor = b;
					}

					void AddMenuItem(int id, std::string content) 
					{
						menuItems[id] = content;
						menuItemCount++;
					}

					void MoveSelectionUp()   { menuItemSelection++; }
					void MoveSelectionDown() { menuItemSelection--; }

					void Show(LibN64::Frame& lFrame) {
						lFrame.DrawRect({pos}, {size}, forecolor);
						lFrame.DrawRect({pos}, {size}, backcolor, false);
						
						lFrame.DrawTextFormat({pos.x+5, pos.y+5}, "test");
						
						int incy = 20;
						for(auto &t : menuItems) {
							lFrame.DrawText({pos.x+5, pos.y+incy}, t.second);
							incy+=10;
						}
					}

					void SetFocused() {
						inFocus = true;
					}
			};

			class LibMenuManager
			{
			
				public:
					std::vector     <LibMenu*>  menuList;
					std::map    <ID, LibMenu*>  menuMap;

					void AddMenu(ID i, std::string t, LibPos p, LibPos s, Color f, Color b)
					{
						LibMenu *tmp = new LibMenu(i, t, p, s, f, b);
						menuList.push_back(tmp);
						menuMap[i] = tmp;
					}

					void Resume() {
						for(auto& menus : menuList)
							menus->inFocus = false; 
					}

					LibMenu* operator [](ID i) {return menuMap[i];}
			};

	};
}
