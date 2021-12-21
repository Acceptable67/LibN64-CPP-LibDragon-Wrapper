#include <libdragon.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "LibN64.h"

#define VARX 0
#define VARY 1
#define VARZ 2

struct Variable 
{
	char *name;
};

struct IntVar : Variable
{
	int value;
	IntVar(char* n, int val) {
		this->name = n;
		this->value = val;
	}
};

struct StringVar : Variable
 {
	char* value; 
	StringVar(char* n, char* val){
		this->name = n; 
		this->value = val;
	} 
};


StringVar sVars[] = 
{
	StringVar((char*)"rom",(char*)"CustomLangParser.z64")
};

IntVar iVars[] = {
	IntVar((char*)"x",0),
	IntVar((char*)"y",0)
};

auto ArraySize = 40;
class LangParser 
{
public:
		unsigned    _varnum = 3;
		unsigned	_svarc = 1;
		unsigned    _ivarc = 2;
		
		void     AssignVar(LibN64::Frame *f, char* vName, unsigned value);
		unsigned FindVarValue(const char* vName);
	
		bool ArgumentInitialized(char* arg);
		bool 	  IsNumber(char* s);
		char*  FilterToken(auto c, char token);
		unsigned   hex2int(char *hex);
		void CheckLineArgs(LibN64::Frame *f, char **args, int argc, int linen);
		void 	  CallLine(LibN64::Frame *f, char **args, int size);
		int 		 Parse(LibN64::Frame *f, auto c, auto length);
		
};

bool LangParser::ArgumentInitialized(char* arg) 
{
	int arg4Int = (*arg    << 24) |
				  (*arg+1) << 16  |
				  (*arg+2) << 8   |
				  (*arg+3);
								  
			if(arg4Int != 0x22232425 && 
			   arg4Int != 0x30313233 &&
			   arg4Int != 0x00010203 &&
			   arg4Int != 0x68696A6B &&
			   arg4Int != 0x32333435 
			 //  arg4Int != 0x797a7b7c
			   ) 
				return true;
			else
				return false;
}

char* LangParser::FilterToken(auto c, char token) 
{
	char *tmp = new char[strlen(c)];
	int p=0;
	for(auto i =c;*i != '\0';i++) {
		if(*i != token) {
			tmp[p] = *i;
			++p;
		}
	}
	tmp[p] = '\0';
	return tmp;
}

bool LangParser::IsNumber(char* s) 
{
		char *ptr = s;
		strtol(ptr, &ptr, 12);
		if(*ptr == '\0')
			return true;
		else  
			return false;
		
}

unsigned LangParser::hex2int(char *hex) {
	int32_t val = 0;
	while (*hex) 
	{
		uint8_t byte = *hex++; 
			if (byte >= '0' && byte <= '9') byte = byte - '0';
			else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
			else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;    
				val = (val << 4) | (byte & 0xF);
			}
	return val;
 }	
		
void itos (char* buf, unsigned val) 
{
	buf = new char[4];
	snprintf(buf, 4, "%d", val);
}


int y = 30;

template<class T>
void CheckKeywordAndCall(char **args, int c, const char* keyword, T func) 
{
		if(!strcmp(args[c], keyword)) {func();}
}

void empty() {}
template<class T, class B>
void CheckVariableArray(char *arg, unsigned c, B t, T condfunc)
{
	for(unsigned reg=0;reg < c;reg++) 
	{
		if(!strcmp(arg, t[reg].name)) 
		{
			condfunc(reg);
		}
	}
}

template<class T, class B, class TB>
void CheckVariableArray(char *arg, unsigned c, B t, T condfunc, TB elsefunc = [](int){})
{
	for(unsigned reg=0;reg < c;reg++) 
	{
		if(!strcmp(arg, t[reg].name)) 
		{
			condfunc(reg);
		}
		else {
			elsefunc(reg);
		}
	}
}

/*The meat of the application*/
unsigned ifLine = 50;
bool ifProceeds = false;
int curArg = 0;

void LangParser::CheckLineArgs(LibN64::Frame *f, char **args, int argc, int linen) {
			
			char* arg1 = args[1];
			char* arg3 = args[3];
			
			fprintf(stderr, "\n");
			for(int c = 0;c<argc;c++) 
			{
				if((unsigned)linen == ifLine + 1 && !ifProceeds)
					break;
					
				fprintf(stderr, "[ITERATION] %d args[%d]=%s\n", c,  c, args[c]);
				
				CheckKeywordAndCall(args, c, "set", [&]() 
				{
					if(*args[c+1] != '[' || *(args[c+1]+11) != ']') {
						__assert("LangParser.h",180,"Missing bracket(s) [SET]");
					}
					auto addrStrip = FilterToken(FilterToken(arg1, '['), ']');
					auto addr = hex2int(addrStrip);
					unsigned val=0;
				
					val = hex2int(args[c+3]);
					CheckVariableArray(arg3, _ivarc, iVars, [&](int reg) 
					{
						val = static_cast<int>(iVars[reg].value);
						fprintf(stderr, "--SET-- VAL %d VAR %s ADDR %08X\n", iVars[reg].value, iVars[reg].name, addr);
					});
					
					
					char *t = (char*)(addr);
					*t = val;
				
					f->DrawTextFormat(0,y,"Set drawn %08X %0d val %02X", t, val, *(char*)(addr));
					y+=10;
				});
		
				CheckKeywordAndCall(args, c, "drawtext", [&]() 
				{
					auto x = atoi(FilterToken(args[c+1],','));
					auto y = atoi(FilterToken(args[c+2],','));
					auto message = FilterToken(args[c+3],'\"');
					
					fprintf(stderr, "%02x %02x %02x %02x\n", *args[c+4], *args[c+4]+1, *args[c+4]+2, *args[c+4]+3);
					
					if(ArgumentInitialized(args[c+4])) 
					    strcat(message, args[c+4]);
		
					if(ArgumentInitialized(args[c+5])) 
						strcat(message, args[c+5]);
					
					
					if(IsNumber(message)) 
					{
						f->DrawTextFormat(x, y, "%02X", hex2int(message));
						debugf("[Draw] IsNumber=true %d\n", hex2int(message));
					} 
					else {
						bool varused = false;
						CheckVariableArray(message, _svarc, sVars, [&](int reg) 
						{
								message = FilterToken(sVars[reg].value, '\"');
								debugf("[Draw] SVAR found %s = %s\n", sVars[reg].name, message);
								f->DrawTextFormat(x, y, "%s", message);
								varused = true;
								return;
						});
						
						CheckVariableArray(message, _ivarc, iVars, [&](int reg) 
						{
								int imessage = iVars[reg].value;
								debugf("[Draw] IVAR found %s = %d\n", iVars[reg].name, imessage);
								f->DrawTextFormat(x, y, "%02X", imessage);
								varused  = true;
								return;
						});
						
						if(varused != true) {
							f->DrawTextFormat(x, y, "%s", message);
							varused = false;
						}
					
					}
				});
				
				if(ifProceeds && (unsigned)linen == ifLine + 1) 
				{
					ifProceeds = false;
				}

				CheckKeywordAndCall(args, c, "if", [&]() 
				{
					ifLine = linen;
					CheckVariableArray(args[c+1], _ivarc, iVars, [&](int reg)
					{
						if(!strcmp(args[c+2], "==")) {
							if((unsigned)iVars[reg].value == hex2int(FilterToken(args[c+3],':'))) 
								ifProceeds = true;
						} else if(!strcmp(args[c+2], "!=")) { 
							if((unsigned)iVars[reg].value != hex2int(FilterToken(args[c+3],':'))) 
								ifProceeds = true;
						} else {
							__assert(__FILE__, __LINE__, "Operation requires comparison of some sort [IF Statement]");
						}
						fprintf(stderr, "[IF] %s == %s [%s] on line %d\n", args[c+1], args[c+3], iVars[reg].name, ifLine);
					});
				});
				
				
				CheckKeywordAndCall(args, c, "var", [&]() 
				{
						auto varname = args[c+1];
						auto value   = args[c+3];
						char *buffer = new char[35];
						if(!strcmp(value, "str")) 
						{
							strcpy(buffer, FilterToken(args[c+4], '\"'));
							if(!IsNumber(buffer)) {
								if(ArgumentInitialized(args[c+5])) 
								{
									strcat(buffer, args[c+5]);
								}
							} else
								__assert(__FILE__, __LINE__, "Variable must contain a string [STR VAR]");
						
							sVars[_svarc] = StringVar(varname, buffer);
													
							fprintf(stderr,"\n--VAR-- %s assinged string val %s\n", sVars[_svarc].name, sVars[_svarc].value);
							_svarc++;
						} 
						
						if (!strcmp(value, "int"))
						{
							if(IsNumber(FilterToken(args[c+4], '\"'))) {
								iVars[_ivarc] = IntVar(varname, hex2int(args[c+4]));
								fprintf(stderr,"\n--VAR-- %s assinged val %d\n",iVars[_ivarc].name, iVars[_ivarc].value);
								_ivarc++;
							} else
								__assert(__FILE__, __LINE__, "Variable must contain an int [INT VAR]");
						}
					
						if (!strcmp(value, "addr"))
						{
							iVars[_ivarc] = IntVar(varname, *(char*)(hex2int(args[c+4])));
							fprintf(stderr,"\n--VAR-- %s assinged val %d\n",iVars[_ivarc].name, iVars[_ivarc].value);
							_ivarc++;
						}
						_varnum++;
				});
				
				CheckKeywordAndCall(args, c, "add", [&]() 
				{
					auto number = args[c+1];
					auto variable = args[c+3];
					int called = 0;
					if(number == NULL || variable == NULL) {
						__assert(__FILE__, __LINE__, "Missing required number or variable [ADD]");
					} 
					
					if(IsNumber(number)) {
						CheckVariableArray(variable, _ivarc, iVars, [&](int reg) 
						{
							iVars[reg].value += hex2int(number);
							fprintf(stderr, "\n--ADD-- %s added to %s\n", number, iVars[reg].name);
							called = 1;
						});
						
						if(called != 1) {
							__assert(__FILE__, __LINE__, "Variable not found [ADD]");
							called = 0;
						}
						
						CheckVariableArray(variable, _svarc, sVars, [&](int reg) 
						{
							__assert(__FILE__, __LINE__, "Variable must be an integer to add [ADD]");
						});
					
					} else
						__assert(__FILE__, __LINE__, "loperand must be an integer [ADD]");
				});

				CheckKeywordAndCall(args, c, "sub", [&]() 
				{
						auto number = args[c+1];
						auto variable = args[c+3];
						if(number == NULL || variable == NULL) 
						{
							__assert(__FILE__, __LINE__, "Missing required number or variable [SUB]");
						} 

					if(IsNumber(number)) {
						CheckVariableArray(variable, _ivarc, iVars, [&](int reg) 
						{
							iVars[reg].value -= hex2int(number);
							fprintf(stderr, "--SUB-- %s subbed from %s\n", number, iVars[reg].name);
						});
						
						CheckVariableArray(variable, _svarc, sVars, [&](int reg) 
						{
							__assert(__FILE__, __LINE__, "Variable must be an integer to sub [SUB]");
						});
					} else
						__assert(__FILE__, __LINE__, "loperand must be an integer [SUB]");
				});
				
				CheckKeywordAndCall(args, c, "debug", [&]() 
				{
					auto message = args[c+1];
					fprintf(stderr, "[strlen of arg 2: %d]\n", strlen(args[c+2]));
					
					if(strlen(args[c+2]) >= 1) {
						strcat(message, "");
						strcat(message, args[c+2]);
					}
					f->DrawText(0,0,FilterToken(message,'"'));
					
				});
				
				if(c!= 3 &&c != 1) 
				{
					CheckVariableArray(args[c], _ivarc, iVars, [&](int reg)
					{
						if(!strcmp(args[c+2], "addr")) {
							iVars[reg].value = *(char*)(hex2int(args[c+3]));
							fprintf(stderr, "\n--%s marked with %02X from %08X--\n", iVars[reg].name, *(char*)(hex2int(args[c+3])), hex2int(args[c+3]));
						} else {	
							iVars[reg].value = hex2int(args[c+2]);
							fprintf(stderr, "\n--%s marked with %d--\n", iVars[reg].name, iVars[reg].value);
						}
					});
				}
				
				if (c != 3 && c != 1) 
				{
					CheckVariableArray(args[c], _svarc, sVars, [&](int reg)
					{
						sVars[reg].value = args[c+2];
						debugf("\n--%s marked with %s--\n", sVars[reg].name, sVars[reg
						].value);
					});
				}

			curArg++;
			}
			curArg = 0;
}
		
void LangParser::CallLine(LibN64::Frame *f, char **args, int size) 
{
	char* argsl[ArraySize];
			
	//auto argsc = 0;
	fprintf(stderr,"\n");
	for (int c = 1;c<size;c++ ) 
	{
		auto argsc = 0;
		auto command = strtok(args[c], " ");
		while(command != NULL) 
		{
			argsl[argsc] = command;
			argsc++;
			fprintf(stderr, "[line %d current:  %s]\n", 	c, command);
			command = strtok(NULL, " ");	
		}
		fprintf(stderr,"\n\n");
		CheckLineArgs(f, argsl, argsc, c);
	}
	curArg = 0;
}
		
int LangParser::Parse(LibN64::Frame *f, auto c, auto length) 
		{	
			auto command = strtok(c, "\n");
			char *args[ArraySize];
			auto y = 80;
			auto  argc=0;
			
			if(strcmp(command,"start")) 
			{
				__assert(__FILE__, __LINE__, "Missing script declaration");
			}
			
			while(command != NULL) 
			{
					args[argc] = command;
					f->DrawText(0,y,command);
					fprintf(stderr, "%s\n", command);
					argc++;
					command = strtok(NULL,"\n");
			
					y+=10;
			}
			CallLine(f, args, argc);
			fprintf(stderr, "\nline called\n");
			
			return 1;
	}
