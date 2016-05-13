#include "Oerror.h"

std::ostream* Oerror::os = nullptr;


// Constructors and destructors
Oerror::Oerror(std::ostream& _os)
{
	os2 = &_os;
}

Oerror::Oerror(Oerror& obj){
	os2 = obj.getOS();
}


// Methods

std::ostream* Oerror::getOS(){
	return os2;
}

void Oerror::setPointOutLocal(std::ostream& _os, const std::string _info){
	os2 = &_os;
	info = _info;
}

void Oerror::setPointOut(std::ostream& _os){
	os = &_os;
}

bool Oerror::showInfo(const char* str){
    if(os == nullptr || !str)
		return false;
	 *os << "Error: "<< str << "\n";
	 return true;
}

bool Oerror::showErrorLocal(const char* str){
    if(os2 == nullptr || !str)
		return false;
    *os2 << "\r";
	setTextStyleOs2(StyleText::BRIGHT, Color::GREEN, Color::NONE);
	*os2 << info;
	setDefaulTextStyleOs2(); 
	*os2 << "\n\t";
	setTextStyleOs2(StyleText::BRIGHT, Color::RED, Color::NONE);
	*os2 << "Error:";
	setDefaulTextStyleOs2();
	*os2 << " " << str;
	addNewLnOs2();
	return true;
}

bool Oerror::showErrorLocal(const std::string str){
    if(os2 == nullptr)
		return false;
	*os2 << "\r";
	setTextStyleOs2(StyleText::BRIGHT, Color::GREEN, Color::NONE);
	*os2 << info;
	setDefaulTextStyleOs2(); 
	*os2 << "\n\t";
	setTextStyleOs2(StyleText::BRIGHT, Color::RED, Color::NONE);
	*os2 << "Error:";
	setDefaulTextStyleOs2();
	*os2 << " " << str;
	addNewLnOs2();
	return true;
}

bool Oerror::showInfoLocal(const std::string comand, const std::string str){
    if(os2 == nullptr)
		return false;
	*os2 << "\r";
	setTextStyleOs2(StyleText::BRIGHT, Color::GREEN, Color::NONE);
	*os2 << info;
	setDefaulTextStyleOs2(); 
	*os2<< "\n\t";
	setTextStyleOs2(StyleText::BRIGHT, Color::BLUE, Color::NONE);
	*os2 << comand;
	setDefaulTextStyleOs2();
	*os2 << ": " << str;
	addNewLnOs2();
	return true;
}

// Operators
Oerror& Oerror::operator=(Oerror& obj){
	os2 = obj.getOS();
	return *this;
}

bool Oerror::showMessage(const std::string&& str, bool option){
    if(os2 == nullptr)
		return false;
	*os2 << "\r";
	*os2 << str;
	if(option)
		addNewLnOs2();
	return true;
}

bool Oerror::showMessageWithTitle(const std::string&& title, const std::string text, bool option){
    if(os2 == nullptr)
		return false;
	*os2 << "\r\t";
	setTextStyleOs2(StyleText::BRIGHT, Color::BLUE, Color::NONE);
	*os2 << title << ":";
	setDefaulTextStyleOs2();
	*os2 << " " << text; 
	if(option)
		addNewLnOs2();
	return true;
}

template <class InIt> 
bool Oerror::showListMessage(std::string&& title, InIt first, InIt last){
    if(os2 == nullptr)
		return false;
	*os2 << "\r\t";
	setTextStyleOs2(StyleText::BRIGHT, Color::WHITE, Color::BLUE);
	*os2 << title;
	setDefaulTextStyleOs2();
	*os2 << "\n";
	int n = 1;
	for(; first != last; ++first){
		*os2 << n << "). " << first->first << "\n";
		n++;
	}
	addNewLnOs2();
	return true;
}

bool Oerror::showListMessage(std::string&& title, std::vector<std::string>& vec){
    if(os2 == nullptr)
		return false;
	*os2 << "\r\t";
	setTextStyleOs2(StyleText::BRIGHT, Color::WHITE, Color::BLUE);
	*os2 << title;
	setDefaulTextStyleOs2();
	*os2 << "\n";
	int n = 1;
	for(auto& it: vec){
		*os2 << n << "). " << it << "\n";
		n++;
	}
	addNewLnOs2();
	return true;
}


void Oerror::setTextStyleOs2(StyleText style, Color textColor, Color backgroundColor){
    if(os2 == nullptr)
		return;
	std::string myStyle = "\x1b[" 
	+ ((style == StyleText::NONE) ? "" : (std::to_string((int)style) + ";"))
	+ ((textColor == Color::NONE) ? "" : (std::to_string(30+(int)textColor) + ";")) 
	+ ((textColor == Color::NONE) ? "" : (std::to_string(40+(int)backgroundColor))) + "m";
	*os2 << myStyle; 
}

void Oerror::setDefaulTextStyleOs2(){
    if(os2 == nullptr)
		return;
	*os2 << "\x1b[0m";
}

void Oerror::addNewLnOs2(){
    if(os2 == nullptr)
		return;
	*os2 << "\n>> ";
	os2->flush();
}
