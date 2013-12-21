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

Cxml::Cxml(std::string _name): fileName(_name)
{
	
}

Cxml::~Cxml()
{
#ifdef DEBUG
	std::cout<<__FILE__<<__LINE__<<std::endl;
#endif 
}

int Cxml::create()
{
	return 0;
}

int Cxml::parse()
{
	return 0;	
}
bool Cxml::validateName()
{
	if(fileName.empty())
		return false;
	
	int len = fileName.size();
	if(fileName.substr(len-3,3) != "xml")
		return false;
		
	return true;
}