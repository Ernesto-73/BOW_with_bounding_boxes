#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

typedef struct 
{
	Mat img;
	std::vector<KeyPoint> keyPoints;
	Mat descriptor;
	Mat BOWDescriptor;
	int label;
}BOWImg;

int imgRead(string path, std::vector<BOWImg> &dst)
{
	int num=0;		
	return num;
}

int main(int argc, char *argv[])
{
	if(argc < 3 )
	{
		std::cout<<std::endl
				 <<"Usage:"<<argv[0]<< " [-p] [-e*] [-c*] [-C*] "<<std::endl
				 <<"		-p Training Path. "<<std::endl
				 <<"		-e Extractor. {SIFT| SURF| Harris}."<<std::endl
				 <<"		-c Classifier. {SVM| GMM| BP}"<<std::endl
				 <<"		-C Cluster. {Kmeans| Kmeans++| GMM}"<<std::endl
				 <<std::endl;
		return EXIT_FAILURE;
	}
		
	string trainingPath("~/BOWImages");
	
	// Default options
	string descriptor("SIFT");	// descriptor
	string extractor("SIFT");	// extractor
	string classifier("SVM");
	string cluster("Kmeans++");
	// Parse options
	for(int i=1;i<argc; i+=2)
	{
		if(i+1 == argc )
			break;
		else {
			if(string(argv[i]) == "-p") {
				trainingPath =string(argv[i+1]);
			}else if(string(argv[i]) == "-e") {
				extractor = string(argv[i+1]);
			}else if(string(argv[i]) == "-c") {
				classifier = string(argv[i+1]);
			}else if(string(argv[i]) == "-C") {
				cluster = string(argv[i+1]);
			}else {
				std::cerr<<"ERROR: Not enough or invalid arguments, please try again.\n"; 
				return EXIT_FAILURE;
			}
		}
	}
	
	// load training files
	std::vector<BOWImg> images;
	int numImages = imgRead(trainingPath,images);
	if(numImages < 0)
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}
