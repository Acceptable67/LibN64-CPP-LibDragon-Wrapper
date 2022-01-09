#include <LibN64.h>
#include <array>

#define MENU mm[MID_MEMPAKTEST]

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
		SMID_DELETE
	};

	bool readEntrySelected  = false;
	bool writeEntrySelected = false;

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

			writeEntrySelected = true;
			pakManager->WriteAnyMemPakEntry((char*)(tmpds));
		});

		MENU->AddMenuItem(SMID_READ, "Read Entry", 
		[&]()
		{
			readEntrySelected = true;
			if(pakManager->IsValid() && pakManager->MemPakInserted()) 
			{
				ds = (DataStruct*)(pakManager->ReadMemPakEntry(pakManager->FindFirstEntryWith(pakManager->GetFileHandle())));
			}
	
		});

		MENU->AddMenuItem(SMID_DELETE, "Delete Entry", 
		[&]()
		{
			
			if(pakManager->IsValid() && pakManager->MemPakInserted()) 
			{
				pakManager->DeleteMemPakEntry(pakManager->FindFirstEntryWith(pakManager->GetFileHandle()));
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
			DrawTextFormat({5,5}, "VALID: %u INSERTED: %u VALID %u FREE %u", pakManager->IsValid(), pakManager->MemPakInserted(), pakManager->GetValidEntries(), pakManager->GetBlocksFree());
		MENU->Show(*this);

		if(readEntrySelected) {
			DrawTextFormat({5,100},"ds->a = %08X\nds->b = %c", ds->a, ds->b);
		}

		if(writeEntrySelected) {
			DrawTextFormat({5,120},"PAK Data written.");
		}

	}

	void KeyAPressed() {
		MENU->WaitKeyPress();
	}

	void KeyJoyYPressed(int direction) {
		switch(direction) {
			case JoyUp:   MENU->MoveSelectionUp(*this);   break;
			case JoyDown: MENU->MoveSelectionDown(*this); break;
			default: break;
		}

	}

};

int main(void) {
	MemPakTest n64g(RESOLUTION_320x240, DEPTH_32_BPP, ANTIALIAS_OFF, Frame::UIType::GUI);
	n64g.Begin();
	
	return 0;
}
