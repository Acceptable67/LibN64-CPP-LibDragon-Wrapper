#include "LibN64.h"

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
	
	template int*      QuickRead<int*>(const char*);
	template char*     QuickRead<char*>(const char*);
	template sprite_t* QuickRead<sprite_t*>(const char*);
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
			rdp_enable_texture_copy();
			rdp_enable_primitive_fill();
			rdp_enable_blend_fill();
			rdp_attach_display(this->d);
			
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
	void LibN64::Frame::KeyJoyPressed(int){}
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

	unsigned LibN64::Frame::ScreenWidth() { return screenWidth; }
	unsigned LibN64::Frame::ScreenHeight() { return screenHeight; }

	void LibN64::Frame::DrawRect(LibPos pos, LibPos dimensions, unsigned  c)
	{
		graphics_draw_box(d, pos.x, pos.y, dimensions.x, dimensions.y, c);
	}

	void LibN64::Frame::Begin() 
	{	
		if(this->d) 
			display_show(d);
		this->OnCreate();

		this->__OnInit_FreeFunction1();
		this->__OnInit_FreeFunction2();

		while (lActive) {
			this->__OnLoop_FreeFunction1();

			this->FrameUpdate();

			controller_scan();
			controller_data keys = get_keys_held();
			
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
				if (keys.c[0].x || 
					keys.c[0].y) 	   { this->KeyJoyPressed(data); }
			}
		
			//display_show(LibN64_Display);
			if(uitype == CONSOLE) {
				console_render();
			}
			this->__OnLoop_FreeFunction2();
		}
}

	void LibN64::Frame::Close() 
	{
		lActive = false;
	}
	void LibN64::Frame::DrawCircle(LibPos pos, int scale, unsigned  c) 
	{
		for(int angles =0;angles<80*scale;angles++) {
			graphics_draw_box(d, pos.x + cosf(angles) * 3.1415f * scale, pos.y + sinf(angles) * 3.1415f * scale, 1, 1, c);
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

	void LibN64::Frame::DrawText(LibPos pos, const char* buf, unsigned c) 
	{
		graphics_set_color(c, 0);
		graphics_draw_text(this->d, pos.x, pos.y, buf);
		graphics_set_color(WHITE, 0);
	}

	float LibN64::Frame::Ticks2Seconds(float t) 
	{
		return (t * 0.021333333 / 1000000.0);
	}
}


