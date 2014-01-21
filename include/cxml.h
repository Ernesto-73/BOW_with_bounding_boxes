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
<<<<<<< HEAD
	int create(std::string fileName);
=======
	
	// output to .xml file 
	int create(std::string fileName);
	
	// add root node 
>>>>>>> origin/new
	int addRoot(Glib::ustring root);
	int parse();
	~Cxml();
	int addItem(Glib::ustring name, Glib::ustring text);
	
	// currently, the depth can only be one
	int addItem(Glib::ustring name, Glib::ustring text);
	
private:
	// .xml file name
	xmlpp::Document document;
	xmlpp::Element* nodeRoot;
	// check if the file name is valid.
<<<<<<< HEAD
	bool validateName(std::string fileName);
=======
	
	// make sure hte file name must have '.xml'
	bool validateName(std::string fileName);
	
	// check if the file exists
>>>>>>> origin/new
	bool fileExists(std::string fileName);
};
#endif