#include <LibN64.h>

using namespace LibN64;

class MenuTest : public Frame  {
 public:
   MenuTest(resolution_t res, bitdepth_t dep, antialias_t aa, UIType ui) : Frame(res, dep, aa, ui) {}

protected:

	LibN64::LibMenuManager mm;
	enum MenuID 
	{
		MID_OPTION
	};

	enum SubMenuID 
	{
		SMID_OPEN,
		SMID_CLOSE,
		SMID_READ,
		SMID_WRITE,
		SMID_CLOSEMENU
	};

	void OnCreate() override 
	{
		SetKeyState(KeysDown);
		SetDLInLoop();

		mm.AddMenu(MID_OPTION, "Options", {30,30}, LibN64::Color::BLACK, LibN64::Color::WHITE);
		mm[MID_OPTION]->AddMenuItem(SMID_OPEN, "LibOpen", 
		[&]()
		{
			DrawText({0,0},"LibOpen Code goes here");
		});
		mm[MID_OPTION]->AddMenuItem(SMID_CLOSE, "LibClose", 
		[&]()
		{
			DrawText({0,10},"LibClose Code goes here");
		});
		mm[MID_OPTION]->AddMenuItem(SMID_READ, "LibRead", 
		[&]()
		{
			DrawText({0,20},"LibReadCode goes here");
		});
		mm[MID_OPTION]->AddMenuItem(SMID_WRITE, "LibWrite", 
		[&]()
		{
			DrawText({0,30},"LibWrite Code goes here");
		});

		mm[MID_OPTION]->AddMenuItem(SMID_CLOSEMENU, "Close", 
		[&]()
		{
			mm[MID_OPTION]->Hide();
		});


		mm[MID_OPTION]->SetFocused();
	}
	

	void FrameUpdate() override 
	{ 
		mm[MID_OPTION]->Show(*this);

	}

	void KeyAPressed() {
		mm[MID_OPTION]->WaitKeyPress();
	}

	void KeyJoyYPressed(int direction) {
		switch(direction) {
			case JoyUp:   mm[MID_OPTION]->MoveSelectionUp();   break;
			case JoyDown: mm[MID_OPTION]->MoveSelectionDown(); break;
		}
	}

};

int main(void) {
	MenuTest n64g(RESOLUTION_320x240, DEPTH_32_BPP, ANTIALIAS_OFF, Frame::UIType::GUI);
	n64g.Begin();
	
	return 0;
}
