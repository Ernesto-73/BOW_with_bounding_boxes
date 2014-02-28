/* 	
	\file: cxml.h
	\author: Javier Zhang 
	\date: 2-17-2014
*/

#include "cxml.h"

/*
	the impletation of class Cxml
*/

Cxml::Cxml()
{
	pNode = NULL;
}

// use root node to initialize a xml creator
Cxml::Cxml(std::string root)
{
	addRoot(root);
}

// add root node before create a xml file 
int Cxml::addRoot(Glib::ustring root)
{
	try{
		nodeRoot = document.create_root_node(root);
		nodeRoot->set_child_text("\n");
	}
	catch(const std::exception& ex)
	{
		std::cerr << "Exception caught: " << ex.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

// add an item(child node) to the xml trees.
int Cxml::addItem(Glib::ustring name, Glib::ustring text)
{
	try{
		xmlpp::Element* nodeChild = nodeRoot->add_child(name);
		nodeChild->add_child_text("\n");
		nodeChild->add_child_text(text);
		nodeChild->add_child_text("\n");
	}
	catch(const std::exception& ex)
	{
		std::cerr << "Exception caught: " << ex.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
	
Cxml::~Cxml()
{
 
}

// create a xml file
int Cxml::create(std::string fileName)
{
	// writing to file 
	assert(validateName(fileName));
	
	try{
		document.write_to_file(fileName);
	}catch(const std::exception& ex)
	{
		std::cerr << "Exception caught: " << ex.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

// this function will create a xml Domparser, details will be included in manual.
bool Cxml::parse(std::string fileName)
{
	if(!fileExists(fileName))
		return false;
	try{
		parser.set_validate(false);
		parser.parse_file(fileName);
		if(parser)
		{
			pNode = parser.get_document()->get_root_node();
		}
	}catch(const std::exception& ex)
	{
		std::cerr << "Exception caught: " << ex.what() << std::endl;
		return EXIT_FAILURE;
	}
	return true;	
}
 	
bool Cxml::fileExists(std::string fileName)
{
	if(access(fileName.c_str(),R_OK | W_OK | F_OK))
	{
		std::string  mesg(strerror(errno));
		std::cout<<__FILE__<<":"<<__LINE__<<" Warning("<<errno<<"): "<<mesg<<" '"<<fileName<<"'"<<std::endl;
		return false;
	}
	else 
		return true;
}

bool Cxml::validateName(std::string fileName)
{
	if(fileName.empty())
		return false;
	
	int len = fileName.size();
	if(fileName.substr(len-3,3) != "xml")
		return false;
		
	return true;
}
