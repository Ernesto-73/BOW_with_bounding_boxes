/* 	
	\file: readdata.h
	\author: Javier Zhang 
	\date: 2-17-2014
*/

#ifndef _READDATA_H_
#define _READDATA_H_

#include <matio.h>
#include "conf.h"

// read annotations files 
bool annoRead(std::string annoName, BOWImg &dst);

// read image 
int imgRead(std::vector<BOWImg> &dst);

template <class Type>
void readNumber(mat_t *matfp, char *varName, std::vector<Type> &dst);


/*
	Impletation of read number from '.mat' file.
	Notice: this function can only read vector or array, cannot read cell or structure.
*/
template <class Type>
void readNumber(mat_t *matfp, char *varName, std::vector<Type> &dst)
{
	matvar_t *matvar = Mat_VarRead(matfp,varName);
 
	if ( NULL == matvar ) {
		std::cerr<<"Variable '"<<varName<<"' not found, or error reading MAT file\n"<<std::endl;
	} else {
		double* auxNumb = (double *)matvar->data;
		int dim = matvar->nbytes / sizeof(double);
 
		for (int i = 0; i < dim; i++)
		{
		 	dst.push_back((Type)auxNumb[i]);
		}
		Mat_VarFree(matvar);
	}
}

#endif
