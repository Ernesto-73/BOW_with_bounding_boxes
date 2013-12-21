/* 	\file: cxml.h
	\author: Javier Zhang 
*/
#ifndef CXML_H
#define CXML_H

#include <string>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <errno.h>
#include <cstring>
#include <libxml++/libxml++.h>

class Cxml
{
public:
	Cxml();
	Cxml(std::string root);
	int create(std::string fileName);
	int addRoot(Glib::ustring root);
	int parse();
	~Cxml();
	int addItem(Glib::ustring name, Glib::ustring text);
	
private:
	// .xml file name
	xmlpp::Document document;
	xmlpp::Element* nodeRoot;
	// check if the file name is valid.
	bool validateName(std::string fileName);
	bool fileExists(std::string fileName);
};
#endif