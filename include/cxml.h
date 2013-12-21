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
/*
	\class Cxml
	used to make the annotations, and the configure files etc.
*/
public:
	Cxml();
	Cxml(std::string root);
	
	// output to .xml file 
	int create(std::string fileName);
	
	// add root node 
	int addRoot(Glib::ustring root);
	int parse();
	~Cxml();
	
	// currently, the depth can only be one
	int addItem(Glib::ustring name, Glib::ustring text);
	
private:
	// .xml file name
	xmlpp::Document document;
	xmlpp::Element* nodeRoot;
	// check if the file name is valid.
	
	// make sure hte file name must have '.xml'
	bool validateName(std::string fileName);
	
	// check if the file exists
	bool fileExists(std::string fileName);
};
#endif