/* 	\file: cxml.h
	\author: Javier Zhang 
*/
#ifndef CXML_H
#define CXML_H

#include <string>
#include <iostream>

class Cxml
{
public:
	Cxml();
	Cxml(std::string _name);
	int create();
	int parse();
	~Cxml();
	
private:
	// .xml file name
	std::string fileName;
	
	// check if the file name is valid.
	bool validateName();
};
#endif