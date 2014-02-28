/* 	
	\file: bow.h
	\author: Javier Zhang 
	\date: 2-24-2014
*/
#include "conf.h"
#include "cxml.h"
#include "readdata.h"
#include <omp.h>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>

using namespace cv;
using namespace std;

extern Config conf;

void features(std::vector<BOWImg> &images, std::string ext, std::string det);

void bowFeatures(std::vector<BOWImg> &images, cv::Mat vocabulary, std::string ext);

double test(cv::Mat &vocabulary, void *src);

bool makeBOWModel(std::vector<BOWImg> &images, cv::Mat &vocabulary, cv::Mat &trainData, cv::Mat &response);

void *train(cv::Mat &trainData, cv::Mat &response);


