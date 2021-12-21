#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class LibString 
{
	public:
		char* str = "default";
		
		int Length() 
		{
			return strlen(str);
		}
		
		void Modify(int pos, char c)
		{
			str[pos] = c;
		} 
		
		int ToInt() 
		{
			return atoi(str);
		}
		
		LibString operator + (char* s) {
			LibString tmp;
			tmp.str = str;
			strcat(tmp.str, s);
			return tmp;
		}
		
		LibString operator = (char* s) {
			LibString tmp;
			tmp.str = s;
			return tmp;
		}
		
		LibString operator + (int s) {
			LibString tmp;
			char *tmpc = (char*)malloc(4);
			tmp.str = str;
			snprintf(tmpc, 4, "%d", s);
			strcat(tmp.str, tmpc);
			return tmp;
		}

		template<typename T>
		operator T() const { return str; }
		operator char* () { return str; }
		
};