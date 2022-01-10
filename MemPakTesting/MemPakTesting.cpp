#include <LibN64.h>

#define MENU mm[MID_MEMPAKTEST]
#define FindFirstEntryWithFileHandle() pakManager->FindFirstEntryWith(pakManager->GetFileHandle())

using namespace LibN64;

class MemPakTest : public Frame  {
 public:
   MemPakTest(resolution_t res, bitdepth_t dep, antialias_t aa, UIType ui) : Frame(res, dep, aa, ui) {}

protected:

	LibN64::LibMenuManager mm; 
	LibN64::LibMemPak *pakManager;

	enum MenuID 
	{
		MID_MEMPAKTEST,
	};

	enum SubMenuID 
	{
		SMID_WRITE,
		SMID_READ,
		SMID_DELETE,
		SMID_FORMAT
	};

	struct DataStruct 
	{
		int a;
		char b;
	};

	DataStruct *ds;
	
	void InitializeMenuSystem() 
	{

		mm.AddMenu(MID_MEMPAKTEST, "LibMemPak Test", {30,30}, LibColor::ORANGE, LibColor::BLACK);

		MENU->AddMenuItem(SMID_WRITE, "Write Entry", 
		[&]()
		{
			DataStruct* tmpds = new DataStruct(0x01020304, 'B');

			pakManager->WriteAnyMemPakEntry(tmpds);
		});

		MENU->AddMenuItem(SMID_READ, "Read Entry", 
		[&]()
		{
			if(pakManager->IsValid() && pakManager->MemPakInserted()) 
			{
				ds = (DataStruct*)(pakManager->ReadMemPakEntry(FindFirstEntryWithFileHandle()));
			}
		});

		MENU->AddMenuItem(SMID_DELETE, "Delete Entry", 
		[&]()
		{
			
			if(pakManager->IsValid() && pakManager->MemPakInserted()) 
			{
				pakManager->DeleteMemPakEntry(FindFirstEntryWithFileHandle());
			}
		});

		MENU->AddMenuItem(SMID_FORMAT, "Format Pak", 
		[&]()
		{
			if(pakManager->IsValid() && pakManager->MemPakInserted()) 
			{
				pakManager->FormatMemPak();
			}
	
		});

		MENU->SetItemSpacing(12);
		MENU->SetFocused();	
		MENU->SetHighlightColor(BLUE);
	}

	void OnCreate() override 
	{
		SetDLInLoop();
		InitializeMenuSystem();

		pakManager = new LibN64::LibMemPak("MEMPAK.Z", 0);
	}

	void FrameUpdate() override 
	{ 
		ClearScreenRDP(LibColor::GREY);

		DrawTextFormat({5,5}, "VALID: %u INSERTED: %u EVALID %u FREE %u", pakManager->IsValid(), pakManager->MemPakInserted(), pakManager->GetValidEntries(), pakManager->GetBlocksFree());
		
		MENU->Show(*this);

		if(MENU->MenuItemIsSelected(SMID_READ))   { DrawTextFormat({5,100},"ds->a = %08X\nds->b = %c", ds->a, ds->b);	}
		if(MENU->MenuItemIsSelected(SMID_WRITE))  { DrawTextFormat({5,120},"PAK Data written."); 					 	}
		if(MENU->MenuItemIsSelected(SMID_FORMAT)) { DrawTextFormat({5,140},"PAK Formatted."); 							}
	}

	void KeyAPressed() {
		MENU->WaitKeyPress();
	}

	void KeyJoyYPressed(int direction) {
	
		switch(direction) 
		{
			case JoyUp:   MENU->MoveSelectionUp  (*this); break;
			case JoyDown: MENU->MoveSelectionDown(*this); break;
			default: break;
		}

	}

};

int main(void) {
	MemPakTest n64g(RESOLUTION_512x240, DEPTH_32_BPP, ANTIALIAS_RESAMPLE, Frame::UIType::GUI);
	n64g.Begin();
	
	return 0;
}
