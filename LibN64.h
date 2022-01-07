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
		
		/**
		 * @brief Opens a DFS file
		 * @param  char* file 
		 * @return handle to the file  
		 */
		static unsigned Open(const char* file) 
		{
			_dfs_handle = dfs_open(file);
			return _dfs_handle;
		}
		
		/**
		 * @brief Grabs the currently open DFS's file size
		 * @return Current size of open file
		 */
		static int Size()
		{
			auto s = dfs_size(_dfs_handle);
			return s;
		}
		
		 /** 
		 * @brief Closes the DFS session 
		 */
		static void Close()
		{
			dfs_close(_dfs_handle);
		}
		
		/** 
		 * @brief Opens a file for reading with a user-provided buffer
		 * @param T buffer
		 * @param unsigned size
		 * @param unsigned offset (to read from)
		 */
		template<class T>
		static void Read(T buf, unsigned size, unsigned offset = 0x0) 
		{
			dfs_seek(_dfs_handle, offset, SEEK_SET);
			if(size > 0)
				dfs_read(buf, 1, size, _dfs_handle);
		}
		
		/**
		 * @brief Opens a file, reads it to a buffer, and returns the buffer contents all in one go.
		 * Great for if you do not need to do anything specifical during the reading.
		 * 
		 * @tparam T specified buffer that will be returned (int, float, char, sprite_t* etc)
		 * @param file file path 
		 * @return T 
		 */
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
	/**
	* @brief Converts seperate R,G,B,A and mixes down into one color 
	* @param  {int} r  
	* @param  {int} g  
	* @param  {int} b 
	* @param  {int} a  
	* @return {int}   
	*/
	constexpr int MakeColor(int r, int g, int b, int a) 
	{
		return (r << 24) | (((g & 0x00FFFFFF) << 16)) | (((b & 0xFF00FFFF) << 8)) | ((a & 0xFFFF00FF));
	}

	enum LibColor 
	{
		 RED			= MakeColor(0xFF, 0x00, 0x00, 0xFF),
		 GREEN			= MakeColor(0x00, 0xFF, 0x00, 0xFF),
		 WHITE			= MakeColor(0xFF, 0xFF, 0xFF, 0xFF),
		 BLACK			= MakeColor(0x00, 0x00, 0x00, 0x00),
		 BLUE			= MakeColor(0x00, 0x00, 0xFF, 0xFF),
		 NAVY_BLUE		= MakeColor(0x11, 0x18, 0x80, 0xFF),
		 DARK_RED		= MakeColor(0x4B, 0x13, 0x00, 0xFF),
		 DEEP_DARK_RED	= MakeColor(0x1F, 0x01, 0x00, 0xFF),
		 SKY_BLUE		= MakeColor(0x11, 0x18, 0xD7, 0xFF),
		 ORANGE			= MakeColor(0xFF, 0xA5, 0x00, 0xFF),
		 GOLD			= MakeColor(0xFF, 0xD7, 0x00, 0xFF),
		 YELLOW			= MakeColor(0xFF, 0xFF, 0x00, 0xFF),
		 CYAN			= MakeColor(0x00, 0xFF, 0xFF, 0xFF),
		 GREY			= MakeColor(0x80, 0x80, 0x80, 0xFF),
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

	template<class SpecifiedType>
	struct Lib2DVec
	{
		std::pair<SpecifiedType, SpecifiedType> values;
	};

	class Frame 
	{
		public:
			virtual void FrameUpdate();
			virtual void OnCreate();
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

		private:
			display_context_t d;
			sprite_t* 		  libFont = nullptr;

			int 	screenWidth;
			int 	screenHeight;

			bool 	bActive;
			bool    bDLInLoop = false;
			float 	fFrameTime;
			float 	fTotalTime;
			float   fFrameRate;
			float   fFrameCounter;
			int     iFrames;

			int 	uitype;

			/*Local RDP function*/
			inline void rdp_quickattach() 
			{
				rdp_init();
				rdp_attach_display(GetDisplay());
				rdp_sync(SYNC_PIPE);
				rdp_set_default_clipping();
				rdp_sync(SYNC_PIPE);
				rdp_enable_blend_fill();
			}

			/*Local RDP Function*/
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
			
			
			/**
			 * @brief Does the dirty work of setting the resolution values
			 * 
			 *
			 * @param  {resolution_t} resolution
			 */
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
					case RESOLUTION_640x240: sr(640, 240); break;
					case RESOLUTION_640x480: sr(640, 480); break;
					case RESOLUTION_512x240: sr(512, 240); break;
					case RESOLUTION_512x480: sr(512, 480); break;
					default: sr(320, 240); break;
				}
			}
			
			/*Empty frame constructor*/
			Frame() {}
			
			/**
			 * @brief Sets up the entire frame with values required to render LibN64 functions
			 *
			 * @param	resolution_t res 
			 * @param	bitdepth_t   dep  
			 * @param   antialias_t   aa
			 *  @param  UIType 		  ui  
			 */
			Frame(resolution_t res, bitdepth_t dep, antialias_t aa, UIType ui) : uitype(ui)
			{
				d = 0;
				kstate = KeysHeld;
				CheckAndSwitchRes(res);
			
				controller_init();
				dfs_init(DFS_DEFAULT_LOCATION);

				switch(ui)
				{
					case GUI:
					{
						display_init(res, dep, 3, GAMMA_NONE, aa);

						while(!(d = display_lock()));

						audio_init(44100, 4);
						mixer_init(16);  
						mixer_ch_set_limits(2, 0, 128000, 0);

						graphics_fill_screen(d, 0x0);
						graphics_set_color(0xFFFFFFFF, 0x0);
					} break;
					case CONSOLE: 
					{
						console_set_render_mode(RENDER_AUTOMATIC);
						console_init();
						console_render();
					} break; 
					default: __assert(__FILE__,__LINE__, "Invalid UI selection (Must be either GUI or Console)"); break;
				}
				bActive = true;
			}

	
			/**
			 * @brief This is called as soon as the frame has been initialized.
			 * 		  The loop is established here.
			 */
			void Begin() 
			{
				if(!bDLInLoop) 
					display_show(d);

				this->OnCreate();

				this->__OnInit_FreeFunction1();
				this->__OnInit_FreeFunction2();
			
				
				while (bActive) 
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

			
			/**
			 * @brief Stop the loop and continue execution in the main()
			 * 
			 */
			void Close() 
			{
				 bActive = false;
			}

			/** @brief Clears the screen with a specified color, selective to what type of GUI mode is enabled.
			 * 			If no color is specified, it is defaulted to black.
			 * @param  unsigned color
			 */
			void ClearScreen(unsigned c = BLACK)
			{
				switch(uitype) 
				{
					case GUI    : graphics_fill_screen(d, c);
					case CONSOLE: console_clear();
				}
			}
			/** @brief Clears the screen with a specified color but with RDP (Hardware)
			 * 
			 * @param  unsigned color 
			 */
			void ClearScreenRDP(unsigned c = BLACK) 
			{
					if(uitype == GUI) 
					{
						DrawRDPRect({0,0},{(int)ScreenWidth(), (int)ScreenHeight()}, c);
					}
			}
			
			/** @brief Changes screen resolution on the fly
			 *  @bug does not work with most emulators
			 * 
			 * @param  {resolution_t} resolution 
			 * @param  {bitdepth_t} bitdepth    
			 */
			void SetScreen(resolution_t res, bitdepth_t bd)
			{
				CheckAndSwitchRes(res);

				display_close();
				display_init(res, bd, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);
				d = display_lock();
			}

			/** @brief Returns the frame's display context
			 * @return display_context_t
			 */
			display_context_t GetDisplay     () { return this->d;};
			
			/**
			 * @brief Sets the key state in the loop (whether it's keydown, pressed, up, or held).
			 * @param  KeyState k
			 */
			void SetKeyState       (KeyState k) { kstate = k;}

			/**
			 * @brief Sets the display_lock inside the loop which is ideal for most applications but some 
			 * such as my CirclesAndSquares demo flicker and refresh oddly with it
			 */
			void SetDLInLoop      			 () { bDLInLoop = true;}
			
			/**
			 * @brief Returns screenwidth 
			 * @return {int}  
			 */
			int  ScreenWidth   				 () { return screenWidth;}

			/**
			 * @brief Returns screenheight 
			 * @return {int}  
			 */
			int  ScreenHeight  				 () { return screenHeight;};

			/** @brief DrawText feature with ability to use as printf
			 * 
			 * @param  LibPos pos         
			 * @param  std::string format 
			 * @param  ... args      
			 */
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

			/** @brief DrawText feature with ability to use as printf with coloring
			 * 
			 * @param  {LibPos} pos         
			 * @param  {std::string} format 
			 * @param  {...} args      
			 */
			void DrawTextFormat(LibPos pos, LibColor forecolor, LibColor backcolor, const std::string format, ...)
			{
				va_list args;
				va_start(args, format.c_str());
				char *buffer = new char[300];
				vsprintf(buffer, format.c_str(), args);
				DrawText(pos, buffer, forecolor, backcolor);
				va_end(args);	

				delete buffer;
			}
	
			/**
			 * @brief Draw's 8x8 text to the screen at the specified location
			 * @param  LibPos position
			 * @param  std::string text    
			 * @param  LibColor forecolor 
			 * @param  LibColor backcolor 
			 */
			void DrawText(LibN64::LibPos pos, const std::string t, LibColor forecolor = WHITE, LibColor backcolor = BLACK) 
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
			/** 
			 * @brief Draws pixel to the screen at the specified location
			 * @param  LibPos pos 
			 * @param  unsigned color
			 */
	 		void DrawPixel(LibPos pos, unsigned c = WHITE) 
			{
				graphics_draw_pixel(this->d, pos.x, pos.y, c);
			}
			
			/** @brief Can draw either a filled or wireframe rectangle at the specified position with specified dimensions
			 * and color
			 * 
			 * @param  {LibPos} pos           
			 * @param  {LibPos} dimensions
			 * @param  {unsigned} color          
			 * @param  {bool} isFilled 
			 */
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

	
			/**
			* @brief 
			* Initializes RDP, draws an RDP Rectangle to the screen
			*
			* @param  {LibPos} pos           
			* @param  {LibPos} dimensions         
			* @param  {unsigned} color          
			*/
			void DrawRDPRect(LibPos pos, LibPos dimensions={1,1}, unsigned c = WHITE) 
			{
				rdp_quickattach();
				rdp_set_blend_color(c);
				rdp_draw_filled_rectangle(pos.x,pos.y,dimensions.x,dimensions.y);
				rdp_quickdetach();
			}

			/**
			 * @brief Draws a line to the screen
			 * 
			 * @param pos1 Start X and Y
			 * @param pos2  End X and Y
			 * @param c Specified color of the line 
			 */
	 		void DrawLine(LibPos pos1, LibPos pos2, unsigned c = WHITE)
			{
				graphics_draw_line(this->d, pos1.x, pos1.y, pos2.x, pos2.y, c);
			}

			/**
			 * @brief Draws a circle to the screen with customizable step size and optional filling and scale
			 * 
			 * @param pos 
			 * @param scale 
			 * @param cStepSize 
			 * @param c 
			 * @param isFilled 
			 */
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

			/**
			 * @brief Draws a software triangle to the screen at the specified location
			 * @param pos1 Point 1
			 * @param pos2 Point 2
			 * @param pos3 Point 3
			 * @param c  Specified color
			 */
			void DrawTriangle(LibPos pos1,LibPos pos2,LibPos pos3, unsigned c = WHITE) 
			{
				DrawLine({pos1.x,pos1.y},{pos2.x,pos2.y}, c);
      			DrawLine({pos2.x,pos2.y},{pos3.x,pos3.y}, c);
      			DrawLine({pos3.x,pos3.y},{pos1.x,pos1.y}, c);
			}

			/**
			 * @brief Draws an RDP (Hardware) triangle to the screen at the specified locations
			 * @param pos1 Point 1
			 * @param pos2 Point 2
			 * @param pos3 Point 3
			 * @param c Specified fill color
			 */
			void DrawRDPTri(LibPos pos1,LibPos pos2,LibPos pos3, unsigned c = WHITE) 
			{
				rdp_quickattach();
				rdp_set_blend_color(c);
				rdp_draw_filled_triangle(pos1.x,pos1.y,pos2.x,pos2.y,pos3.x,pos3.y);
				rdp_quickdetach();
			}

			/**
			 * @brief Draws a sprite (sprite_t*) at the specified location
			 * @param pos 
			 * @param spr 
			 */
			void DrawSprite(LibPos pos, sprite_t* spr) {
				graphics_draw_sprite(this->d, pos.x, pos.y, spr);
			}

			/**
			 * @brief Converts ticks to seconds for easy readability 
			 * @param float ticks 
			 * @return float 
			 */
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

			void DrawTextCF(LibPos pos, const std::string str, int LibColor = WHITE) 
			{
				unsigned incx = pos.x;
				unsigned incy = pos.y;
				for(size_t index = 0;index<str.length();index++) 
				{
					if(incx >= ScreenWidth()) { incy+=8; incx = pos.x; }

					Graphics::DrawFont(GetDisplay(), incx, incy, LibColor, libFont, str[index]);
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
			LibN64::ID		mId;
			LibN64::LibPos 	mPos;
			std::string 	mTitle;
			std::string 	mContent;
			LibN64::LibColor	mForecolor, mBackcolor;

			int         mMenuItemSpacing = 10;
			int         mMenuItemCount;
			float		mMenuItemSelection;

			std::map   <int,      std::string> mMenuItems;
			std::vector<std::function<void()>> mMenuItemCallbacks;

			bool bMenuIsShowing   = true;
			bool bEnableHighlight = true;
			bool bKeyStateHeld    = true;
			LibColor cHighlightColor = LibColor::RED;
		public:
			bool 		bInFocus;		
		public:
			LibMenu(ID i, std::string t, LibPos p, LibColor f = BLACK, LibColor b = WHITE)
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

			void MoveSelectionUp(Frame &r)   { 
				if((mMenuItemSelection - 1) >= 0 && bInFocus && MenuIsShowing()) 
					mMenuItemSelection-= (r.kstate == Frame::KeyState::KeysHeld || r.kstate == Frame::KeyState::KeysPressed) ? 0.02 : 1; 
			}

			void MoveSelectionDown(Frame &r) { 
				if((mMenuItemSelection + 1) < mMenuItemCount && bInFocus && MenuIsShowing()) 
					mMenuItemSelection+= (r.kstate == Frame::KeyState::KeysHeld || r.kstate == Frame::KeyState::KeysPressed) ? 0.02 : 1; 
			}

			void Show(LibN64::Frame& lFrame)
			{
				lFrame.SetKeyState(Frame::KeyState::KeysDown);

				if(bMenuIsShowing) 
				{
					auto findLargestMenuItem = [&]() -> int
					{
						std::vector<int> stringLengths;
						for(auto& t : mMenuItems) {
							stringLengths.push_back(t.second.length());
						}
						stringLengths.push_back(this->mTitle.length());

						std::sort(stringLengths.begin(), stringLengths.end(), [](int a, int b) {
							return a > b;
						});

						return stringLengths.front();
					};
					
					auto toUpper = [&](std::string str) -> std::string
					{
						std::string tmp;
						for(auto& character : str) {
							tmp += toupper(character);
						}
						return tmp;
					};

					LibPos dimensions = 
					{ 
						10 * findLargestMenuItem(), 
						35 + (10 * mMenuItemCount)
					};
					
					
					lFrame.DrawRect({mPos}, dimensions, mForecolor);
					lFrame.DrawRect({mPos}, dimensions, mBackcolor, false);
					lFrame.DrawRect({mPos}, {dimensions.x, 15}, mBackcolor);
					lFrame.DrawText({mPos.x+5, mPos.y+5},  mTitle, mForecolor, mBackcolor);

					int incy = 20, spot = 0;
					for(auto &t : mMenuItems) 
					{
						if(bInFocus && (int)mMenuItemSelection == spot) 
						{
							if(bEnableHighlight) 
							{
								lFrame.DrawRect({mPos.x+2, mPos.y+incy-2}, {dimensions.x-3, 10}, cHighlightColor);
							}
							lFrame.DrawText({mPos.x+5, mPos.y+incy}, toUpper(t.second));
						} 
						else 
							lFrame.DrawText({mPos.x+5, mPos.y+incy}, t.second);
						incy+=mMenuItemSpacing;
						++spot;
					}
				}
			}

			bool MenuIsShowing() {
				return bMenuIsShowing;
			}

			void Hide() {
		
				bMenuIsShowing = false;
				SetUnfocused();
			}

			void EnableShow() {
				bMenuIsShowing = true;
			}

			void EnableHighlight() {
				bEnableHighlight = true;
			}

			void DisableHighlight() {
				bEnableHighlight = false;
			}

			void SetHighlightColor(LibColor c) {
				cHighlightColor = c;
			}

			void WaitKeyPress() {
				if(bInFocus)
				 {
					mMenuItemCallbacks.at(mMenuItemSelection)();
				}
			}
			void SetItemSpacing(int spacing) {
				mMenuItemSpacing = spacing;
			}

			void SetFocused() {
				bInFocus = true;
			}

			void SetUnfocused() {
				bInFocus = false;
			}
	};

	class LibMenuManager
	{
		private:
			std::vector     <LibMenu*>  menuList;
			std::map    <ID, LibMenu*>  menuMap;

		public:
			void AddMenu(ID i, std::string t, LibPos p, LibColor f, LibColor b)
			{
				LibMenu *tmp = new LibMenu(i, t, p, f, b);
				menuList.push_back(tmp);
				menuMap[i] = tmp;
			}

			bool AllMenusClosed() {
				for(auto &menus : menuList) {
					if(menus->MenuIsShowing() || menus->bInFocus) {
						return false;
					}
				}

				return true;
			}

			void CloseFocusedMenus() {
				for(auto& menus : menuList) {
					if(menus->bInFocus) {
						menus->Hide();
					}
				}
			}

			void CloseAllMenus() {
				for(auto& menus : menuList) {
					menus->Hide();
				}
			}

			LibMenu* operator [](ID i) {return menuMap[i];}
	};


	/*having issues*/
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


