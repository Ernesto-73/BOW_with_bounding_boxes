#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <cstdlib>
#include "SampleSet.h"
#include "cxml.h"

using namespace std;

int main()
{	
	SampleSet tarinval, testval;
	
	tarinval.addDir("/home/javier/bow/images/training/airplanes","airplane");
	tarinval.addDir("/home/javier/bow/images/training/bottles","bottle");
	tarinval.addDir("/home/javier/bow/images/training/camel","camel");
	tarinval.addDir("/home/javier/bow/images/training/car_side","car");
	tarinval.addDir("/home/javier/bow/images/training/guitars","guitar");
	tarinval.addDir("/home/javier/bow/images/training/humanfaces","humanface");
	tarinval.addDir("/home/javier/bow/images/training/leaves","leave");
	tarinval.addDir("/home/javier/bow/images/training/motorbikes","motorbike");
	tarinval.setUsePrefix(false);
	tarinval.rename();
	
	tarinval.createList("/home/javier/bow/imgset/tranval");
	tarinval.makeAnnotations("/home/javier/bow/Annotations");
	std::cout<<"Total: "<<tarinval.getTotalNum()<<std::endl;
	
	return EXIT_SUCCESS;
}

