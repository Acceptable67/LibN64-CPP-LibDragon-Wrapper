#include <LibN64.h>

namespace LibN64::DFS 
{
	int handle = 0;
	
	unsigned Open(const char* file) 
	{
		handle = dfs_open(file);
		return handle;
	}
	
	int Size()
	{
	  	auto s = dfs_size(handle);
	  	return s;
	}
	
	template<class T>
	void Read(T buf, unsigned size, unsigned offset) 
	{
	    dfs_seek(handle, offset, SEEK_SET);
	    if(size > 0)
			dfs_read(buf, 1, size, handle);
	}
	
	template<class T>
 	T QuickRead(const char* file) 
 	{
 		Open(file);
 		
 		T buffer = (T)malloc(Size());
 		
 		Read(buffer, Size());
 		Close();
 		
 		return buffer;
 	}

	void Close()
	{
		dfs_close(handle);
	}

	template void Read<int*>(int*,unsigned,unsigned);
	template void Read<char*>(char*,unsigned,unsigned);
	template void Read<sprite_t*>(sprite_t*,unsigned,unsigned);
	template void Read<Sprite*>(Sprite*,unsigned,unsigned);
	
	template int*      QuickRead<int*>(const char*);
	template char*     QuickRead<char*>(const char*);
	template sprite_t* QuickRead<sprite_t*>(const char*);
	template Sprite* QuickRead<Sprite*>(const char*);
};

namespace LibN64 
{
    void LibN64::Frame::CheckAndSwitchRes(resolution_t r) 
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

    LibN64::Frame::Frame(resolution_t res=RESOLUTION_320x240, bitdepth_t dep=DEPTH_32_BPP, antialias_t aa=ANTIALIAS_RESAMPLE, UIType ui = GUI) 
    {
		uitype = ui;
		d = 0;
		kstate = KeysHeld;
		CheckAndSwitchRes(res);
	
		controller_init();
		dfs_init(DFS_DEFAULT_LOCATION);

		if(ui == GUI) 
		{
			display_init(res, dep, 2, GAMMA_NONE, aa);

			while (!(d = display_lock()));

			rdp_init();
			rdp_sync(SYNC_PIPE);
			rdp_set_default_clipping();
			rdp_enable_primitive_fill();
			rdp_enable_blend_fill(); 
			rdp_attach_display(this->d); 
			rdp_sync(SYNC_PIPE);

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

	void LibN64::Frame::ClearScreen() 
	{
		switch(uitype) {
			case GUI    : graphics_fill_screen(d, 0x0);
			case CONSOLE: console_clear();
		}
	}

	void LibN64::Frame::ClearScreenRDP() 
	{
			if(uitype == GUI) 
				DrawRDPRect({0,0},{(int)ScreenWidth(), (int)ScreenHeight()}, 0x0);
	}

	void LibN64::Frame::SetScreen(resolution_t resol, bitdepth_t bd)
	{
		CheckAndSwitchRes(resol);

		display_close();
		display_init(resol, bd, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);
		d = display_lock();
	}

	void LibN64::Frame::SwitchBuffer(display_context_t buffer) 
	{
		display_close();
		display_init(RESOLUTION_320x240, DEPTH_32_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);
		buffer = display_lock();
		
	}

	unsigned LibN64::Frame::ScreenWidth() { return screenWidth; }
	unsigned LibN64::Frame::ScreenHeight() { return screenHeight; }

	display_context_t LibN64::Frame::GetDisplay() {
		return this->d;
	}

	void LibN64::Frame::DrawRect(LibPos pos, LibPos dimensions, unsigned  c, bool isFilled)
	{
		if(isFilled) {
			graphics_draw_box(d, pos.x, pos.y, dimensions.x, dimensions.y, c);
		} else {
			DrawLine( pos, {pos.x + dimensions.x, pos.y}, c);
			DrawLine({pos.x + dimensions.x, pos.y}, {pos.x+dimensions.x, pos.y + dimensions.y}, c);
			DrawLine({pos.x+dimensions.x, pos.y+dimensions.y}, {pos.x, pos.y + dimensions.y}, c);
			DrawLine({pos.x, pos.y + dimensions.y}, pos, c);
		}
	}

	void LibN64::Frame::Begin() 
	{	
		if(this->d) 
			display_show(d);
		this->OnCreate();

		this->__OnInit_FreeFunction1();
		this->__OnInit_FreeFunction2();
	

		while (lActive) {
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
			
				//display_show(LibN64_Display);
				if(uitype == CONSOLE) {
					console_render();
				}
				fFrameTime = timer_ticks();

				fFrameTime -= fTmp;
				fFrameCounter += fFrameTime; 
				fTotalTime += fFrameTime;
				timer_close();

				this->__OnLoop_FreeFunction2();
		}
	}

	float LibN64::Frame::GetTotalTime() { return Ticks2Seconds(fTotalTime);}
	float LibN64::Frame::GetFrameTime() { return Ticks2Seconds(fFrameTime);}
	float LibN64::Frame::GetFrameRate() { return (1 / GetFrameTime()); }

	void LibN64::Frame::SetControllerScanRate(float fRate) {controllerScanRate = fRate;}
	void LibN64::Frame::SetKeyState(KeyState k) {kstate = k;}

	void LibN64::Frame::Close() 
	{
		lActive = false;
	}
	void LibN64::Frame::DrawCircle(LibPos pos, int scale, float cStepSize, unsigned c, bool isFilled) 
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

	void LibN64::Frame::DrawRDPRect(LibPos pos, LibPos dimensions, unsigned c)
	{
		rdp_set_blend_color(c);
		rdp_draw_filled_rectangle(pos.x,pos.y,dimensions.x,dimensions.y);
	}

	void LibN64::Frame::DrawTriangle(LibPos pos1, LibPos pos2, LibPos pos3, unsigned c)
  	{
     	DrawLine({pos1.x,pos1.y},{pos2.x,pos2.y}, c);
      	DrawLine({pos2.x,pos2.y},{pos3.x,pos3.y}, c);
      	DrawLine({pos3.x,pos3.y},{pos1.x,pos1.y}, c);
 	 }

	void LibN64::Frame::DrawRDPTri(LibPos pos1, LibPos pos2, LibPos pos3, unsigned c) 
	{
		rdp_set_blend_color(c);
		rdp_draw_filled_triangle(pos1.x,pos1.y,pos2.x,pos2.y,pos3.x,pos3.y);
	}

	void LibN64::Frame::DrawLine(LibPos pos1, LibPos pos2, unsigned c) 
	{
		graphics_draw_line(this->d, pos1.x, pos1.y, pos2.x, pos2.y, c);
	}

	void LibN64::Frame::DrawPixel(LibPos pos, unsigned c) 
	{
		graphics_draw_pixel(this->d, pos.x, pos.y, c);
	}

	void LibN64::Frame::DrawSprite(LibPos pos, sprite_t* spr) {
		graphics_draw_sprite(this->d, pos.x, pos.y, spr);
	}

	void LibN64::Frame::DrawText(LibPos pos, const std::string buf, unsigned forecolor, unsigned backcolor) 
	{
		if(forecolor != WHITE || backcolor != 0x0) {
			graphics_set_color(forecolor, backcolor);
			graphics_draw_text(this->d, pos.x, pos.y, buf.c_str());
			graphics_set_color(WHITE, 0);
		} else
			graphics_draw_text(this->d, pos.x, pos.y, buf.c_str());
	}


	void LibN64::Frame::DrawTextFormat(LibPos pos,const std::string format, ...) {
		va_list args;
		va_start(args, format.c_str());
		char *buffer = new char[300];
		vsprintf(buffer, format.c_str(), args);
		DrawText(pos, buffer);
		va_end(args);	
		delete buffer;
	}

	void LibN64::Frame::DrawTextFormat(LibPos pos, unsigned forecolor, unsigned backcolor, const std::string format, ...) {
		va_list args;
		va_start(args, format.c_str());
		char *buffer = new char[300];
		vsprintf(buffer, format.c_str(), args);
		DrawText(pos, buffer, forecolor, backcolor);
		va_end(args);	
		delete buffer;
	}


	float LibN64::Frame::Ticks2Seconds(float t) 
	{
		return (t * 0.021333333 / 1000000.0);
	}

	void LibN64::Frame::LoadCustomFont(const std::string FileName) 
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

	void LibN64::Frame::DrawTextCF(LibPos pos, const std::string str, int color) {
		int incx = pos.x;
		int incy = pos.y;
		for(int index = 0;index<str.length();index++) 
		{
			if(incx >= ScreenWidth()) { incy+=8; incx = pos.x; }

		 	Graphics::DrawFont(GetDisplay(), incx, incy, color, libFont, str[index]);
			incx += 8;
		}
	}

	void LibN64::Frame::DrawTextFormatCF(LibPos pos, const std::string format, ...)
	{
		va_list args;
		va_start(args, format.c_str());
		char buffer[300];
		vsprintf(buffer, format.c_str(), args);
		DrawTextCF(pos, buffer);
		va_end(args);	
	}


}


