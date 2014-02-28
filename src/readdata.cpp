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
	char varName[] = "box_coord";
	readNumber<int>(matfp,varName,dst.box);
	//readNumber<double>(matfp,"obj_contour",dst.objContour);
	
	Mat_Close(matfp);
	return true;
}

int imgRead(std::vector<BOWImg> &dst)
{
	int num=0;
	for(unsigned i=0;i<conf.classes.size();i++)
	{
		std::string path = conf.path+"/Images"+"/"+conf.classes[i];
		if(access(path.c_str(),R_OK | W_OK | F_OK))
		{
			std::cerr<<"Error: Directory '"<<path<<"' does not exist."<<std::endl;
			return -1;
		}
		for(int j=0;j<conf.max_num;j++)
		{
			char buf[64];
			sprintf(buf,"image_%04d.jpg",j+1);
			std::string imgName = conf.path+"/Images"+"/"+conf.classes[i]+"/"+std::string(buf);
		
			sprintf(buf,"annotation_%04d.mat",j+1);
			std::string annoName = conf.path+"/Annotations"+"/"+conf.classes[i]+"/"+std::string(buf);
		
			BOWImg bowImg;
		
			bowImg.img = cv::imread(imgName,0);
			if(!bowImg.img.data)
				break;
			annoRead(annoName,bowImg); // fill up box && objContour
			bowImg.imgName = imgName;
			bowImg.label = i+1;
			dst.push_back(bowImg);
			num++;
		}
	}
	return num;
}


