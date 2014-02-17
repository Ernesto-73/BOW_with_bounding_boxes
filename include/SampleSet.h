/* 	
	\file: SampleSet.h
	\author: Javier Zhang 
	\date: 1-21-2014
*/

#ifndef _SAMPLESET_H_
#define _SAMPLESET_H_

// these are the unix system call headers
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <errno.h>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include "cxml.h"

typedef struct dirent *DStream;
/*
	\class SampleSet
	Create the sample set, inlcuding rename file, create file list and annotations etc.
*/
class SampleSet
{
public:
	SampleSet();
	SampleSet(bool _usePrefix, int _start = 0);
	~SampleSet();
	
	// add paths, doesn't call rename
	void addDir(std::string _path, std::string _tag);
	
	// main function: rename file
	void rename();
	
	// translate the int to string
	std::string num2str(int num);
	
	// create file list without suffix
	void createList(std::string listName);
	
	// make annotations
	void makeAnnotations(std::string _path);
	// set && get 
	void setUsePrefix(bool val);
	void setPrefix(std::string _prefix);
	unsigned getTotalNum();
	
private:
	std::vector<std::string> paths;	// sample directory
	std::vector<std::string> tags;	//class name
	
	bool exisitsOrMkdir(std::string _path);
	
	bool usePrefix;	// if the new file name use prefix
	int start;	
	std::string prefix;	// file name prefix
	unsigned num;
	
	// handling the suffix
	std::string getSuffix(const std::string &name);
	std::string stripSuffix(const std::string &name);
};
#endif 
