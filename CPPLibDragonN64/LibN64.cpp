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
	  	return dfs_size(handle);
	}
	
	void Read(char* buf, unsigned size, unsigned offset) 
	{
	    dfs_seek(handle, offset, SEEK_SET);
	    if(size > 0) 
		dfs_read(buf, 1, size, handle);
	}
	
 	char* QuickRead(const char* file) 
 	{
 		Open(file);
 		
 		char * buffer = new char[Size()];
 		
 		Read(buffer, Size());
 		Close();
 		
 		return buffer;
 	}
	
	void Close()
	{
		dfs_close(handle);
	}
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

    LibN64::Frame::Frame(resolution_t res=RESOLUTION_320x240, bitdepth_t dep=DEPTH_32_BPP) 
    {
		d = 0;
	
		CheckAndSwitchRes(res);
	
		controller_init();
		dfs_init(DFS_DEFAULT_LOCATION);
		display_init(res, dep, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);

		while (!(d = display_lock()));
	
		graphics_fill_screen(d, 0x0);
		graphics_set_color(0xFFFFFFFF, 0x0);

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
	void LibN64::Frame::KeyZPressed()  {}
	void LibN64::Frame::OnCreate()     {}

	void LibN64::Frame::ClearScreen() 
	{
		graphics_fill_screen(d, 0x0);
	}

	void LibN64::Frame::SetScreen(resolution_t resol, bitdepth_t bd)
	{
		CheckAndSwitchRes(resol);

		display_close();
		display_init(resol, bd, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);
		d = display_lock();
	}

/*void LibN64::Frame::DrawFrame() {
	for (int h = 0; h < screenHeight; h++) {
		for (int w = 0; w < screenWidth; w++) {
			graphics_draw_box(LibN64_Display, w, h, 1,1,screenBuf[h * screenWidth + w]);

		}
	}
}*/

	unsigned LibN64::Frame::ScreenWidth() { return screenWidth; }

	unsigned LibN64::Frame::ScreenHeight() { return screenHeight; }


	void LibN64::Frame::DrawBox(int x, int y, int scale, unsigned  c)
	{
		graphics_draw_box(d, x, y, scale, scale, c);
	}

	void LibN64::Frame::Begin() 
	{	
		display_show(d);
		this->OnCreate();
		while (lActive) {
			timer_init();
			this->FrameUpdate();

			controller_scan();
			controller_data keys = get_keys_down();

			if (keys.c[0].err == ERROR_NONE) {
				if (keys.c[0].A) {
					this->KeyAPressed();
				}
				if (keys.c[0].B) {
					this->KeyBPressed();
				}
				if (keys.c[0].up) {
					this->KeyDUPressed();
				}
				if (keys.c[0].down) {
					this->KeyDDPressed();
				}
				if (keys.c[0].left) {
					this->KeyDLPressed();
				}
				if (keys.c[0].right) {
					this->KeyDRPressed();
				}
				if (keys.c[0].Z) {
					this->KeyZPressed();
				}
				if (keys.c[0].start) {
					this->KeyStartPressed();
				}
			}
		
			//display_show(LibN64_Display);

			fFrameTime = timer_ticks();
			fTotalTime += fFrameTime;
			timer_close();
		}
}

	void LibN64::Frame::Close() 
	{
		lActive = false;
	}
	void LibN64::Frame::DrawCircle(int x, int y, int scale, unsigned  c) 
	{
		for(int angles =0;angles<80*scale;angles++) {
			graphics_draw_box(d, x + cosf(angles) * 3.1415f * scale, y + sinf(angles) * 3.1415f * scale, 1, 1, c);
		}
	}
	void LibN64::Frame::DrawLine(int x1, int y1, int x2, int y2, unsigned c) 
	{
		graphics_draw_line(this->d, x1, y1, x2, y2, c);
	}

	void LibN64::Frame::DrawPixel(int x, int y, unsigned c) 
	{
		graphics_draw_pixel(this->d, x, y, c);
	}

	void LibN64::Frame::DrawText(int x, int y, const char* buf, unsigned c) 
	{
		graphics_set_color(c, 0);
		graphics_draw_text(this->d, x, y, buf);
		graphics_set_color(WHITE, 0);
	}

	float LibN64::Frame::Ticks2Seconds(float t) 
	{
		return (t * 0.021333333 / 1000000.0);
	}
}


