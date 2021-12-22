#include "LibString.h"

int LibString::Length() 
{
	return strlen(str);
}
		
void LibString::Modify(int pos, char c)
{
	str[pos] = c;
} 
		
int LibString::ToInt() 
{
	return atoi(str);
}
		
LibString LibString::operator + (char* s) {
	LibString tmp;
	tmp.str = str;
	strcat(tmp.str, s);
	return tmp;
}
		
LibString LibString::operator = (char* s) {
	LibString tmp;
	tmp.str = s;
	return tmp;
}
		
LibString LibString::operator + (int s) {
	LibString tmp;
	char *tmpc = (char*)malloc(4);
	tmp.str = str;
	snprintf(tmpc, 4, "%d", s);
	strcat(tmp.str, tmpc);
	return tmp;	}