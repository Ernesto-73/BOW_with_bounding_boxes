/* 	
	\file: conf.h
	\author: Javier Zhang 
	\date: 2-17-2014
*/

#ifndef _CONF_H_
#define _CONF_H_

#include <fstream>
#include <opencv2/opencv.hpp>
#include <termios.h>
#include "cxml.h"

// configuration of BOW model
typedef struct {
	std::string path;	
	std::string extractor;
	std::string detector;
	std::string cluster;
	std::string classifier;
	int numClusters;
	int max_num;
	std::vector<std::string> classes;
}Config;

// BOW data structure
typedef struct {
	cv::Mat img;
	std::string imgName;
	std::vector<cv::KeyPoint> keyPoints;
	cv::Mat descriptor;
	cv::Mat BOWDescriptor;
	int label;
	std::vector<int> box;
	//std::vector<double> objContour; 
}BOWImg;

std::string trim(std::string s, const char* t =" \t\n\r\f\v");

bool configuration(int argc, char *argv[]);

static void restore_terminal_settings(void);

static void disable_terminal_return(void);
#endif
