#pragma once

#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wnarrowing"

#include <libdragon.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <vector>
#include <list>
#include <functional>
#include <map>
#include <stdio.h>
#include <iostream>

#define TICKS_TOTAL(since_start)	(timer_ticks()-since_start) * 0.021333333 / 1000000.0

typedef char byte;
class		 LibMenuManager;
class		 LibMenu;

static int _dfs_handle;
namespace LibN64 
{
	class DFS 
	{
		public:
		
		static unsigned Open(const char* file) 
		{
			_dfs_handle = dfs_open(file);
			return _dfs_handle;
		}
		
		static int Size()
		{
			auto s = dfs_size(_dfs_handle);
			return s;
		}
		
		static void Close()
		{
			dfs_close(_dfs_handle);
		}
		
		template<class T>
		static void Read(T buf, unsigned size, unsigned offset = 0x0) 
		{
			dfs_seek(_dfs_handle, offset, SEEK_SET);
			if(size > 0)
				dfs_read(buf, 1, size, _dfs_handle);
		}
		
		template<class T>
		static T QuickRead(const char* file) 
		{
			Open(file);
			
			T buffer = (T)malloc(Size());
			
			Read(buffer, Size());
			Close();
			
			return buffer;
		}

	};
};

namespace LibN64 
{	
	constexpr int MakeColor(int r, int g, int b, int a) 
	{
		return (r << 24) | (((g & 0x00FFFFFF) << 16)) | (((b & 0xFF00FFFF) << 8)) | ((a & 0xFFFF00FF));
	}

	enum Color 
	{
		 RED			= MakeColor(0xFF,0x00,0x00,0xFF),
		 GREEN			= MakeColor(0x00, 0xFF, 0x00, 0xFF),
		 WHITE			= MakeColor(0xFF, 0xFF, 0xFF, 0xFF),
		 BLACK          = MakeColor(0x00, 0x00, 0x00, 0x00),
		 BLUE           = MakeColor(0x00, 0x00, 0xFF, 0xFF),
		 NAVY_BLUE		= MakeColor(17, 24, 128,255),
		 DARK_RED		= MakeColor(75, 19, 0,255),
		 DEEP_DARK_RED	= MakeColor(31, 1, 0,255),
		 SKY_BLUE		= MakeColor(17, 24, 215,255),
		 ORANGE			= MakeColor(0xFF, 0xA5, 0x00, 0xFF),
		 GOLD			= MakeColor(0xFF, 0xD7, 0x00, 255),
		 YELLOW			= MakeColor(0xFF, 0xFF, 0x00, 255),
		 CYAN			= MakeColor(0x00, 0xFF, 0xFF, 255),
		 GREY			= MakeColor(0x80, 0x80, 0x80, 255),
		 PURPLE			= MakeColor(0xFF, 0x00, 0x9B, 0xFF)
	};

	struct LibPos 
	{ 
		int x, y; 
		LibPos operator +(LibPos x) const 
		{
			LibPos tmp = { this->x, this->y };
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

	class Frame 
	{
		public:
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

			int screenWidth;
			int screenHeight;

		private:
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

			inline void rdp_quickattach() 
			{
				rdp_init();
				rdp_attach_display(GetDisplay());
				rdp_sync(SYNC_PIPE);
				rdp_set_default_clipping();
				rdp_sync(SYNC_PIPE);
				rdp_enable_blend_fill();
			}

			inline void rdp_quickdetach()
			{
				rdp_sync(SYNC_PIPE);
				rdp_detach_display();
				rdp_close();
			}
		
		public:
			enum     UIType        { GUI, CONSOLE };
			enum     Joystick      { JoyUp=0x00000072, JoyDown=0x0000008E, JoyLeft=0x00008E00, JoyRight=0x00007200 };
			enum 	 KeyState 	   { KeysHeld, KeysDown, KeysPressed, KeysUp } kstate;
			
			void CheckAndSwitchRes(resolution_t r) 
			{
				auto sr = [&](int x, int y)  
				{
					this->screenWidth = x;
					this->screenHeight = y;
				};

				switch (r) 
				{
					case RESOLUTION_320x240: sr(320, 240); break;
					case RESOLUTION_256x240: sr(256, 240); break;
					case RESOLUTION_640x480: sr(640, 480); break;
					case RESOLUTION_512x480: sr(512, 480); break;
					default: sr(320, 240); break;
				}
			}
			
			Frame() {}
			Frame(resolution_t res, bitdepth_t dep, antialias_t aa, UIType ui)
			{
				uitype = ui;
				d = 0;
				kstate = KeysHeld;
				CheckAndSwitchRes(res);
			
				controller_init();
				dfs_init(DFS_DEFAULT_LOCATION);

				if(ui == GUI) 
				{
					display_init(res, dep, 3, GAMMA_NONE, aa);

					while(!(d = display_lock()));

					audio_init(44100, 4);
					mixer_init(16);  
					mixer_ch_set_limits(2, 0, 128000, 0);

					graphics_fill_screen(d, 0x0);
					graphics_set_color(0xFFFFFFFF, 0x0);
					}
				else 
				if(ui == CONSOLE) 
				{
					console_set_render_mode(RENDER_AUTOMATIC);
					console_init();
					console_render();
				} else 
					__assert(__FILE__,__LINE__, "Invalid UI selection (Must be either GUI or Console)");

				lActive = true;
			}

			virtual void OnCreate  ();

			void Begin() 
			{
				if(!bDLInLoop) 
					display_show(d);

				this->OnCreate();

				this->__OnInit_FreeFunction1();
				this->__OnInit_FreeFunction2();
			
				
				while (lActive) 
				{
					if(bDLInLoop)
					 {
						d = display_lock();
						rdp_attach_display(d);
					}

					timer_init();
					float fTmp = timer_ticks();
					this->__OnLoop_FreeFunction1();
				
				
					this->FrameUpdate();

						controller_scan();
						controller_data keys;
						switch(kstate)
						{
							case KeysHeld:    keys = get_keys_held(); break;
							case KeysDown:    keys = get_keys_down(); break;
							case KeysPressed: keys = get_keys_pressed(); break;
							case KeysUp:      keys = get_keys_up(); break;
							default: break;
						};
						
						if (keys.c[0].err == ERROR_NONE) {
							int data = keys.c[0].data;
						
							if (keys.c[0].A)       { this->KeyAPressed();    }
							if (keys.c[0].B)       { this->KeyBPressed();    }
							if (keys.c[0].up)      { this->KeyDUPressed();   }
							if (keys.c[0].down)    { this->KeyDDPressed();   }
							if (keys.c[0].left)    { this->KeyDLPressed();   }
							if (keys.c[0].right)   { this->KeyDRPressed();   }
							if (keys.c[0].Z)       { this->KeyZPressed();    }
							if (keys.c[0].start)   { this->KeyStartPressed();}
							if (keys.c[0].C_up)    { this->KeyCUPressed();   }
							if (keys.c[0].C_down)  { this->KeyCDPressed();   }
							if (keys.c[0].C_left)  { this->KeyCLPressed();   }
							if (keys.c[0].C_right) { this->KeyCRPressed();   }
							if (keys.c[0].x)	   { this->KeyJoyXPressed(data & 0x0000FF00); } 
							if (keys.c[0].y) 	   { this->KeyJoyYPressed(data & 0x000000FF); }
						}
					
						if(uitype == CONSOLE) {
							console_render();
						}
						fFrameTime = timer_ticks();

						fFrameTime -= fTmp;
						fFrameCounter += fFrameTime; 
						fTotalTime += fFrameTime;
						timer_close();	
						if(bDLInLoop) display_show(d);
						this->__OnLoop_FreeFunction2();
				}
			}

			void Close() 
			{
				 lActive = false;
			}

			void ClearScreen() 
			{
				switch(uitype) 
				{
					case GUI    : graphics_fill_screen(d, 0x0);
					case CONSOLE: console_clear();
				}
			}

			void ClearScreenRDP(unsigned c = BLACK) 
			{
					if(uitype == GUI) 
					{
						DrawRDPRect({0,0},{(int)ScreenWidth(), (int)ScreenHeight()}, c);
					}
			}

			void SetScreen(resolution_t res, bitdepth_t bd)
			{
				CheckAndSwitchRes(res);

				display_close();
				display_init(res, bd, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);
				d = display_lock();
			}

			display_context_t GetDisplay     () { return this->d;};
			void SetKeyState       (KeyState k) { kstate = k;}
			void SetDLInLoop      			 () { bDLInLoop = true;}
			int ScreenWidth   				 () { return screenWidth;}
			int ScreenHeight  				 () { return screenHeight;};

			void DrawTextFormat(LibPos pos, const std::string format, ...) 
			{
				va_list args;
				va_start(args, format.c_str());
				char *buffer = new char[300];
				vsprintf(buffer, format.c_str(), args);
				DrawText(pos, buffer);
				va_end(args);	
				
				delete buffer;
			}

			void DrawTextFormat(LibPos pos, Color forecolor, Color backcolor, const std::string format, ...)
			{
				va_list args;
				va_start(args, format.c_str());
				char *buffer = new char[300];
				vsprintf(buffer, format.c_str(), args);
				DrawText(pos, buffer, forecolor, backcolor);
				va_end(args);	
				delete buffer;
			}

			void DrawText(LibN64::LibPos pos, const std::string t, Color forecolor = WHITE, Color backcolor = BLACK) 
			{
				if(forecolor != WHITE || backcolor != 0x0) 
				{
					graphics_set_color(forecolor, backcolor);
					graphics_draw_text(this->d, pos.x, pos.y, t.c_str());
					graphics_set_color(WHITE, 0);
				} 
				else
					graphics_draw_text(this->d, pos.x, pos.y, t.c_str());
			}

	 		void DrawPixel(LibPos pos, unsigned c = WHITE) 
			{
				graphics_draw_pixel(this->d, pos.x, pos.y, c);
			}

			void DrawRect(LibPos pos, LibPos dimensions={1,1}, unsigned c = WHITE, bool isFilled = true) 
			{
				if(isFilled) 
				{
					graphics_draw_box(d, pos.x, pos.y, dimensions.x, dimensions.y, c);
				} 
				else {
					DrawLine( pos, {pos.x + dimensions.x, pos.y}, c);
					DrawLine({pos.x + dimensions.x, pos.y}, {pos.x+dimensions.x, pos.y + dimensions.y}, c);
					DrawLine({pos.x+dimensions.x, pos.y+dimensions.y}, {pos.x, pos.y + dimensions.y}, c);
					DrawLine({pos.x, pos.y + dimensions.y}, pos, c);
				}
			}

			void DrawRDPRect(LibPos pos, LibPos dimensions={1,1}, unsigned c = WHITE) 
			{
				rdp_quickattach();
				rdp_set_blend_color(c);
				rdp_draw_filled_rectangle(pos.x,pos.y,dimensions.x,dimensions.y);
				rdp_quickdetach();
			}

	 		void DrawLine(LibPos pos1, LibPos pos2, unsigned c = WHITE)
			{
				graphics_draw_line(this->d, pos1.x, pos1.y, pos2.x, pos2.y, c);
			}

			void DrawCircle(LibPos pos, int scale=1,float cStepSize=0.1,unsigned c = WHITE, bool isFilled = true) 
			{
				if(isFilled) 
				{
					for(float scaler = 0;scaler<=scale;scaler+=0.3) {
						for(float angles =0;angles<25*scaler;angles+=cStepSize) 
							DrawPixel({pos.x + cosf(angles) * 3.1415f * scaler, pos.y + sinf(angles) * 3.1415f * scaler}, c);
					}
				} else {
					for(float angles =0;angles<25*scale;angles+=0.1) 
						DrawPixel({pos.x + cosf(angles) * 3.1415f * scale, pos.y + sinf(angles) * 3.1415f * scale}, c);
				}
			}

			void DrawTriangle(LibPos pos1,LibPos pos2,LibPos pos3, unsigned c = WHITE) 
			{
				DrawLine({pos1.x,pos1.y},{pos2.x,pos2.y}, c);
      			DrawLine({pos2.x,pos2.y},{pos3.x,pos3.y}, c);
      			DrawLine({pos3.x,pos3.y},{pos1.x,pos1.y}, c);
			}

			void DrawRDPTri(LibPos pos1,LibPos pos2,LibPos pos3, unsigned c = WHITE) 
			{

				rdp_draw_filled_triangle(pos1.x,pos1.y,pos2.x,pos2.y,pos3.x,pos3.y);
				rdp_sync(SYNC_PIPE);
				rdp_detach_display();
				rdp_close();
			}

			void DrawSprite(LibPos pos, sprite_t* spr) {
				graphics_draw_sprite(this->d, pos.x, pos.y, spr);
			}

			float  Ticks2Seconds (float t) 
			{
				return (t * 0.021333333 / 1000000.0);
			}

			/*uses Baremetal-MIPS 8x8 font from PeterLemon (github)*/
			void LoadCustomFont(const std::string FileName) 
			{
				libFont = DFS::QuickRead<sprite_t*>(FileName.c_str());
				if(libFont == nullptr) {
					const std::string err = "There was an error loading the font.";
					if(uitype == GUI) 
						DrawText({0,0},err);
					else 
						printf(err.c_str());
				}
			}

			void DrawTextCF(LibPos pos, const std::string str, int color = WHITE) 
			{
				unsigned incx = pos.x;
				unsigned incy = pos.y;
				for(size_t index = 0;index<str.length();index++) 
				{
					if(incx >= ScreenWidth()) { incy+=8; incx = pos.x; }

					Graphics::DrawFont(GetDisplay(), incx, incy, color, libFont, str[index]);
					incx += 8;
				}
			}

			void DrawTextFormatCF(LibPos pos, const std::string format, ...) 
			{
				va_list args;
				va_start(args, format.c_str());
				char buffer[300];
				vsprintf(buffer, format.c_str(), args);		
				DrawTextCF(pos, buffer);
				va_end(args);	
			}

			float        GetTotalTime() { return Ticks2Seconds(fTotalTime);}
			float        GetFrameTime() { return Ticks2Seconds(fFrameTime);};
			float        GetFrameRate() { return (1 / GetFrameTime()); };

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

	/*menu system*/
	typedef int ID;
	class LibMenu 
	{
		private:
			ID          mId;
			LibPos 		mPos;
			std::string mTitle;
			std::string mContent;
			Color 		mForecolor, mBackcolor;

			int         mMenuItemSpacing = 10;
			int         mMenuItemCount;
			int			mMenuItemSelection;

			std::map   <int,      std::string> mMenuItems;
			std::vector<std::function<void()>> mMenuItemCallbacks;

			bool bMenuIsShowing = true;
		public:
			bool 		bInFocus;		
		public:
			LibMenu(ID i, std::string t, LibPos p, Color f = BLACK, Color b = WHITE)
			{ 
				mId = i; mPos = p; mTitle = t; mForecolor = f; mBackcolor = b;
			}

			template<class FunctionCallback>
			void AddMenuItem(int mId, std::string content, FunctionCallback call) 
			{
				mMenuItems[mId] = content;
				mMenuItemCallbacks.push_back(call);
				mMenuItemCount++;

			}

			void MoveSelectionUp()   { 
				if((mMenuItemSelection - 1) >= 0) 
					mMenuItemSelection--; 
			}

			void MoveSelectionDown() { 
				if((mMenuItemSelection + 1) < mMenuItemCount) 
					mMenuItemSelection++; 
			}

			void Show(LibN64::Frame& lFrame)
				{
				if(bMenuIsShowing) 
				{
					auto findLargestMenuItem = [&]() 
					{
						std::vector<int> stringLengths;
						for(auto& t : mMenuItems) {
							stringLengths.push_back(t.second.length());
						}

						std::sort(stringLengths.begin(), stringLengths.end());

						return stringLengths.back();
					};
					
					auto toUpper = [&](std::string str) 
					{
						std::string tmp;
						for(auto& character : str) {
							tmp += toupper(character);
						}
						return tmp;
					};

					LibPos dimensions = { (10 * findLargestMenuItem()), 35 + (10 * mMenuItemCount)};
					
					lFrame.DrawRect({mPos}, dimensions, mForecolor);
					lFrame.DrawRect({mPos}, dimensions, mBackcolor, false);
					lFrame.DrawRect({mPos}, {dimensions.x, 15}, mBackcolor);
					lFrame.DrawText({mPos.x+5, mPos.y+5},  mTitle, mForecolor, mBackcolor);

					int incy = 20, spot = 0;
					for(auto &t : mMenuItems) 
					{
						lFrame.DrawText({mPos.x+5, mPos.y+incy},
						(bInFocus == true && mMenuItemSelection == spot) ? toUpper(t.second) : t.second);
						incy+=mMenuItemSpacing;
						++spot;
					}
				}
			}

			void Hide() {
				bMenuIsShowing = false;
			}

			void EnableShow() {
				bMenuIsShowing = true;
			}

			void WaitKeyPress() {
				mMenuItemCallbacks.at(mMenuItemSelection)();
			}
			void SetItemSpacing(int spacing) {
				mMenuItemSpacing = spacing;
			}

			void SetFocused() {
				bInFocus = true;
			}
	};

	class LibMenuManager
	{
		private:
			std::vector     <LibMenu*>  menuList;
			std::map    <ID, LibMenu*>  menuMap;

		public:
			void AddMenu(ID i, std::string t, LibPos p, Color f, Color b)
			{
				LibMenu *tmp = new LibMenu(i, t, p, f, b);
				menuList.push_back(tmp);
				menuMap[i] = tmp;
			}

			void Resume() {
				for(auto& menus : menuList)
					menus->bInFocus = false; 
			}

			LibMenu* operator [](ID i) {return menuMap[i];}
	};


	/*cannot compile for some reason*/
	namespace Audio 
	{
		
		class WavAudio
		{
			public:
				wav64_t wTrack;
				int 	iLocalChannel;
				WavAudio(){}
				WavAudio(std::string sTrackTitle) {
					Init(sTrackTitle);
				}

				void Init(std::string sTrackTitle) {
					wav64_open(&wTrack, sTrackTitle.c_str());
					iLocalChannel = 0;
				}

				void Play() {
					wav64_play(&wTrack, 0);
				}

				void SetVolume() {
					mixer_ch_set_vol(iLocalChannel, 0.25f, 0.25f);
				}
		};
	};
}

/* initialize the functions for override as initializing in class seems to be broken*/
namespace LibN64 
{
	void LibN64::Frame::FrameUpdate()  {}
	void LibN64::Frame::KeyAPressed()  {}
	void LibN64::Frame::KeyStartPressed(){}
	void LibN64::Frame::KeyBPressed()  {}
	void LibN64::Frame::KeyDUPressed() {}
	void LibN64::Frame::KeyDDPressed() {}
	void LibN64::Frame::KeyDLPressed() {}
	void LibN64::Frame::KeyDRPressed() {}
	void LibN64::Frame::KeyCUPressed() {}
	void LibN64::Frame::KeyCDPressed() {}
	void LibN64::Frame::KeyCLPressed() {}
	void LibN64::Frame::KeyCRPressed() {}
	void LibN64::Frame::KeyZPressed()  {}
	void LibN64::Frame::KeyJoyXPressed(int){}
	void LibN64::Frame::KeyJoyYPressed(int){}
	void LibN64::Frame::OnCreate()     {}
	inline void LibN64::Frame::__OnInit_FreeFunction1() {}
	inline void LibN64::Frame::__OnInit_FreeFunction2() {}
	inline void LibN64::Frame::__OnLoop_FreeFunction1() {}
	inline void LibN64::Frame::__OnLoop_FreeFunction2() {}
}


