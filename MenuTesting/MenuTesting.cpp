#include <LibN64.h>
#include <array>

using namespace LibN64;

class MenuTest : public Frame  {
 public:
   MenuTest(resolution_t res, bitdepth_t dep, antialias_t aa, UIType ui) : Frame(res, dep, aa, ui) {}

protected:

	LibN64::LibMenuManager mm;
	LibN64::LibMenu *menu; /*easy access pointer*/

	enum MenuID 
	{
		MID_OPTION,
		MID_NEW
	};

	enum SubMenuID 
	{
		SMID_OPEN,
		SMID_CLOSE,
		SMID_READ,
		SMID_WRITE,
		SMID_CLOSEMENU
	};

	std::array<std::string, 4>	menuStrings = 
	{ 
		"LibItem Enabled",
		"LibItem 1 Enabled", 
		"LibItem 2 Enabled",
		"LibItem 3 Enabled"
	};

	void InitializeMenuSystem() 
	{
		mm.AddMenu(MID_OPTION, "Options", {30,30}, LibColor::BLACK, LibColor::WHITE);
	
		menu = mm[MID_OPTION];
		menu->AddMenuItem(SMID_OPEN, "LibItem", 
		[&]()
		{
			/*put optional code to call here, otherwise use the FrameUpdate loop to call
			if the menu is selected or not*/
		});

		menu->AddMenuItem(SMID_CLOSE,		"LibItem 1");
		menu->AddMenuItem(SMID_READ,		"LibItem 2");
		menu->AddMenuItem(SMID_WRITE,		"LibItem 3");
		menu->AddMenuItem(SMID_CLOSEMENU,	"Close", [&](){
			menu->Hide();
		});

		menu->SetItemSpacing(14);
		menu->SetFocused();	
	}

	void OnCreate() override 
	{
		SetKeyState(KeysHeld);
		SetDLInLoop();
		
		InitializeMenuSystem();
	}

	void FrameUpdate() override 
	{ 

		ClearScreenRDP(LibColor::GREY);

		menu->Show(*this);
	
		int y = 10;
		for(auto spot = (size_t)0; spot < menuStrings.size(); spot++) 
		{
			if(menu->MenuItemIsSelected(spot)) 
			{
				DrawText({120, y}, menuStrings[spot]);
				y+=12;
			}
		}

	}

	void KeyAPressed() {
		menu->WaitKeyPress();
	}

	void KeyBPressed() {
		mm.CloseAllMenus();
	}

	void KeyJoyYPressed(int direction) {
		switch(direction) {
			case JoyUp:   menu->MoveSelectionUp(*this);   break;
			case JoyDown: menu->MoveSelectionDown(*this); break;
		}

	}

};

int main(void) {
	MenuTest n64g(RESOLUTION_320x240, DEPTH_32_BPP, ANTIALIAS_OFF, Frame::UIType::GUI);
	n64g.Begin();
	
	return 0;
}
