/* 	
	\file: cxml.h
	\author: Javier Zhang 
	\date: 2-17-2014
*/

#ifndef _CXML_H_
#define _CXML_H_

#include <string>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <errno.h>
#include <cstring>

/* 
	used libxml++ 2.0
	 --libs --cflags libxml++-2.6 
*/
#include <libxml++/libxml++.h>

/*
	this class is both a xml creator and parser.
*/
class Cxml
{
public:
	Cxml();
	
	// use root node to initialize a xml creator
	Cxml(std::string root);
	
	// create a xml file
	int create(std::string fileName);
	
	// add root node before create a xml file 
	int addRoot(Glib::ustring root);
	
	// add an item(child node) to the xml trees.	
	int addItem(Glib::ustring name, Glib::ustring text);
	
	// this function will create a xml Domparser, details will be included in manual.
	bool parse(std::string fileName);

	//	return the root node. We can use this to get a certain value from xml trees.
	const xmlpp::Node *getpNode() const { return pNode;}
	
	~Cxml();
private:
	// dom parser
	xmlpp::DomParser parser;
	
	const xmlpp::Node *pNode; // used for parsing xml
	
	xmlpp::Document document;
	xmlpp::Element* nodeRoot;
	
	// check if the file name is valid.
	bool validateName(std::string fileName);
	
	// check if the file exists.
	bool fileExists(std::string fileName);
};
#endif