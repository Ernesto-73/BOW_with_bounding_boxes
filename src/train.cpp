/* 	
	\file: train.cpp
	\author: Javier Zhang 
	\date: 2-24-2014
*/

#include <sys/time.h>
#include "bow.h"
#include "conf.h"

using namespace std;
using namespace cv;

Config conf;

double timeval_diff(struct timeval *a, struct timeval *b)
{
	return (double)(a->tv_sec + (double)a->tv_usec/1000000) - (double)(b->tv_sec + (double)b->tv_usec/1000000);
}

int main(int argc, char *argv[])
{
	// BOW configurations.
	printf("\033c\n");
	std::cout<<"- BOW Configuration"<<std::endl;
	if(!configuration(argc,argv))
		return EXIT_FAILURE;
		
	// Counting elapsed time.
	struct timeval t_ini, t_fin;
	double secs;
	gettimeofday(&t_ini, NULL);
	
	std::vector<BOWImg> images;
	Mat vocabulary;
	Mat response;
	Mat trainData;
	printf("\n");
	std::cout<<"- BOW Training"<<std::endl;
	if(!makeBOWModel(images, vocabulary, trainData, response))
		return	EXIT_FAILURE;
	void *classifier = train(trainData,response);
		
	printf("\n");
	std::cout<<"- BOW Testing"<<std::endl;
	test(vocabulary, classifier);
	gettimeofday(&t_fin, NULL);
	
	// Number of secs taken to load the whole model
	printf("\n");
	secs = timeval_diff(&t_fin, &t_ini);
	std::cout<<"------------------------------------------------------------------------------------------------"<<std::endl;
	std::cout<<"Elapsed time: "<<secs<<" seconds"<<std::endl;
	std::cout<<"Accurate rate: "<<"%"<<std::endl;
	std::cout<<"------------------------------------------------------------------------------------------------"<<std::endl<<std::endl;
	return EXIT_SUCCESS;
}
