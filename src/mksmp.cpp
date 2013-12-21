#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <cstdlib>
#include "SampleSet.h"

using namespace std;

int main()
{	
	SampleSet test;
	test.addDir("/home/javier/101_ObjectCategories/airplanes","airplanes");
	test.setUsePrefix(true);
	test.setPrefix("cls_1_");
	//test.rename();
	//test.createList("tranval");
	
	return EXIT_SUCCESS;
}

