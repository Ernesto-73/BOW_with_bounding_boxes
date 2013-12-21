/* 	\file: cxml.h
	\author: Javier Zhang 
*/

#include "cxml.h"

/*
	the impletation of class SampleSet
*/

Cxml::Cxml()
{

}

Cxml::Cxml(std::string root)
{
	addRoot(root);
}

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
#ifdef DEBUG
	std::cout<<__FILE__<<__LINE__<<std::endl;
#endif 
}

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

int Cxml::parse()
{
	return 0;	
}

bool Cxml::fileExists(std::string fileName)
{
	if(access(fileName.c_str(),R_OK | W_OK | F_OK))
	{
		std::string  mesg(strerror(errno));
		std::cout<<"Error("<<errno<<"): "<<mesg<<std::endl;
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