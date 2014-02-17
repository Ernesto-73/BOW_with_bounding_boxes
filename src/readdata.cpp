/* 	
	\file: readdata.cpp
	\author: Javier Zhang 
	\date: 2-17-2014
*/

#include "readdata.h"

extern Config conf;
bool annoRead(std::string annoName, BOWImg &dst)
{
	mat_t *matfp = Mat_Open(annoName.c_str(),MAT_ACC_RDONLY);

	if(matfp == NULL)
	{
	    std::cerr<<"Cannot open file '"<<annoName<<"'."<<std::endl;
	    return false;
	}
	readNumber<int>(matfp,"box_coord",dst.box);
	//readNumber<double>(matfp,"obj_contour",dst.objContour);

	Mat_Close(matfp);
	return true;
}

int imgRead(std::vector<BOWImg> &dst)
{
	int num=0;
	std::cout<<std::endl;
	for(unsigned i=0;i<conf.classes.size();i++)
	{
		std::cout<<"Reading class --> "<<conf.classes[i]<<std::endl;
		for(int j=0;j<conf.max_num;j++)
		{
			char buf[64];
			sprintf(buf,"image_%04d.jpg",j+1);
			std::string imgName = conf.trainingPath+"/Images"+"/"+conf.classes[i]+"/"+std::string(buf);
		
			sprintf(buf,"annotation_%04d.mat",j+1);
			std::string annoName = conf.trainingPath+"/Annotations"+"/"+conf.classes[i]+"/"+std::string(buf);
		
			BOWImg bowImg;
		
			annoRead(annoName,bowImg); // fill up box && objContour
			bowImg.img = cv::imread(imgName,0);
			bowImg.imgName = imgName;
			bowImg.label = i;
			dst.push_back(bowImg);
			num++;
		}
	}
	std::cout<<std::endl;
	return num;
}

std::string trim(std::string s, const char* t)
{
	s.erase(0, s.find_first_not_of(t));
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}
