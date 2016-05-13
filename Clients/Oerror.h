#ifndef _OERROR_H_
#define _OERROR_H_

#include <ostream>
#include <vector>

enum class Color
{
	BLACK	= 0,
 	RED		= 1,
	GREEN	= 2,
	YELLOW 	= 3,
	BLUE	= 4,
	MAGENTA	= 5,
	CYAN	= 6,
	WHITE	= 7,
	NONE
};

enum class StyleText
{
	RESET 		= 0,
	BRIGHT 		= 1,
	DIM 		= 2,
	UNDERLINE 	= 3,
	BLINK 		= 4,
	REVERSE 	= 7,
	HIDDEN 		= 8,
	NORMAL_INTENSITY = 21,
	NONE
};

class Oerror
{
private:
	static std::ostream* os;
    std::ostream *os2 = nullptr;
	std::string info;

	void setTextStyleOs2(StyleText, Color, Color);
	void setDefaulTextStyleOs2();
public:
	void addNewLnOs2();
	
	static void setPointOut(std::ostream& _os);
	static bool showInfo(const char* str);
	Oerror(std::ostream& _os);
	Oerror(){}
	Oerror(Oerror&);
	void setPointOutLocal(std::ostream& _os, const std::string _info);

	bool showErrorLocal(const char* str);
	bool showErrorLocal(const std::string str);

	bool showInfoLocal(const std::string comand, const std::string str);

	bool showMessage(const std::string&& str, bool option = true);
	bool showMessageWithTitle(const std::string&& title, const std::string text, bool option = true);

	template <class InIt> 
	bool showListMessage(std::string&&, InIt, InIt);
	bool showListMessage(std::string&&, std::vector<std::string>&);
	std::ostream* getOS();
	Oerror& operator=(Oerror&);
};

#endif // _OERROR_H_
