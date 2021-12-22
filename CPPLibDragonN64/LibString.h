#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class LibString 
{
	public:
		char* str = "default";
		
		int  Length();
		void Modify(int pos, char c);
		int  ToInt();
		
		LibString operator + (char* s);
		LibString operator = (char* s);
		LibString operator + (int s);

		template<typename T>
		operator T() const { return str; }
		operator char* () { return str; }
		
};
