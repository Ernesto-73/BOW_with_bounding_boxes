/* 	
	\file: bow.cpp
	\author: Javier Zhang 
	\date: 2-24-2014
*/
#include "bow.h"

void features(std::vector<BOWImg> &images, cv::string ext, cv::string det)
{	
   	cv::initModule_nonfree();
  	Ptr<FeatureDetector> detector = FeatureDetector::create(det);
	Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create(ext);
#ifdef _USE_OPENMP_
	#pragma omp parallel for
	for(unsigned i = 0;i<images.size();i++)
	{
		cv::Mat img = cv::Mat(images[i].img,cv::Rect(images[i].box[2],images[i].box[0],images[i].box[3]-images[i].box[2],images[i].box[1]-images[i].box[0]));
		detector->detect(img,images[i].keyPoints);
		extractor->compute(img,images[i].keyPoints,images[i].descriptor);
	}
#else
	for(std::vector<BOWImg>::iterator iter = images.begin();iter != images.end(); iter++)
	{
		cv::Mat img = cv::Mat(iter->img,cv::Rect(iter->box[2],iter->box[0],iter->box[3]-iter->box[2],iter->box[1]-iter->box[0]));
		detector->detect(img,iter->keyPoints);
		extractor->compute(img,iter->keyPoints,iter->descriptor);
	}
#endif
}

void bowFeatures(std::vector<BOWImg> &images, cv::Mat vocabulary, std::string ext)
{
	cv::initModule_nonfree();
	Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create(ext);
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");

	BOWImgDescriptorExtractor bow(extractor,matcher);
	bow.setVocabulary(vocabulary);
#ifdef _USE_OPENMP_
	#pragma omp parallel for
	for(unsigned i = 0;i<images.size();i++)
	{
		bow.compute(images[i].img,images[i].keyPoints,images[i].BOWDescriptor);
		
		// To Do: Need normalization or not ?
		normalize(images[i].BOWDescriptor,images[i].BOWDescriptor,1.0,0.0,NORM_MINMAX);
	}
#else
	for(std::vector<BOWImg>::iterator iter = images.begin();iter != images.end(); iter++)
	{
		bow.compute(iter->img,iter->keyPoints,iter->BOWDescriptor);
		
		// To Do: Need normalization or not ?
		normalize(iter->BOWDescriptor,iter->BOWDescriptor,1.0,0.0,NORM_MINMAX);
	}
#endif
}

double test(cv::Mat &vocabulary, void *src)
{
	// Test	
	std::vector<BOWImg> images;
	conf.max_num = conf.max_num * 2;
	
	std::cout<<"--->Loading testing images ... "<<std::endl;
	int numImages = imgRead(images);
	std::cout<<"    "<<numImages<<" images loaded."<<std::endl;
	if(numImages < 0)
		return -1;
		
	printf("--->Extracting %s features ...\n", conf.extractor.c_str());	
	features(images, conf.extractor, conf.detector);
	
	std::cout<<"--->Extracting BOW features ..."<<std::endl;
	bowFeatures(images, vocabulary, conf.extractor);
	
	cv::Mat rawData;
	for(std::vector<BOWImg>::iterator iter = images.begin();iter != images.end(); iter++)
		rawData.push_back(iter->BOWDescriptor);
		
	//PCA	
#ifdef _USE_PCA_
	float factor = 1;
	int maxComponentsNum = static_cast<float>(conf.numClusters) * factor;
	cv::PCA pca(rawData, Mat(),CV_PCA_DATA_AS_ROW, maxComponentsNum);
	cv::Mat pcaData;
	for(int i = 0;i<rawData.rows;i++)
	{
		cv::Mat vec = rawData.row(i);
		cv::Mat coeffs = pca.project(vec);
		pcaData.push_back(coeffs);
	}	
	cv::Mat testData = pcaData;
#else
	cv::Mat testData = rawData;
#endif

	std::cout<<"--->Executing predictions ..."<<std::endl;
	cv::Mat output;
	double ac = 0;
	double ac_rate = 0;
	if(conf.classifier == "BP")
	{
		CvANN_MLP *classifier = (CvANN_MLP *)src;
		classifier->predict(testData,output);
		cout<<"--->Predict answer: "<<std::endl;
		for(int i = 0;i < output.rows;i++)
		{
			float *p = output.ptr<float>(i);
			int k = 0;
			int tmp = 0;
			for(int j = 0;j < output.cols;j++)
			{	
				if(p[j] > tmp )
				{
					tmp = p[j];
					k = j;
				}
			}
			std::cout<<"    "<<images[i].imgName<<" ---- "<<conf.classes[k]<<endl;
			if(images[i].label == k+1)
				ac++;
		}
		ac_rate = ac / static_cast<double>(output.rows);
	}
	else if(conf.classifier == "SVM")
	{
		CvSVM *classifier = (CvSVM *)src;
		classifier->predict(testData,output);
		cout<<"--->Predict answer: "<<std::endl;
		for(int i = 0;i < output.rows;i++)
		{
			int k = (int)output.ptr<float>()[i]-1;
			std::cout<<"    "<<images[i].imgName<<" ---- "<<conf.classes[k]<<endl;
			if(images[i].label == k+1)
				ac++;
		}
		ac_rate = ac / static_cast<double>(output.rows);
	}
	else {
		std::cout<<"--->Error: wrong classifier."<<std::endl;
	}
	return ac_rate;
}
 
bool makeBOWModel(std::vector<BOWImg> &images, Mat &vocabulary, Mat &trainData, Mat &response)
{
	// Load training images
	std::cout<<"--->Loading training images ... "<<std::endl;
	int numImages = imgRead(images);
	if(numImages < 0)
		return false;
	std::cout<<"    "<<numImages<<" images loaded."<<std::endl;
	
	// Random shuffle samples
	std::random_shuffle (images.begin(), images.end());
	
	/*
		Get the ROI of images. 
		Feature detect and extract descriptors.
	*/
	printf("--->Extracting %s features ...\n", conf.extractor.c_str());	
	features(images, conf.extractor, conf.detector);
	
	BOWKMeansTrainer trainer(conf.numClusters,TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,10,1.0));
	for(std::vector<BOWImg>::iterator iter = images.begin();iter != images.end(); iter++)
	{
		Mat tmp = iter->descriptor;
		trainer.add(tmp);
	}
	
	std::cout<<"--->Constructing vocabulary list ..."<<std::endl;	
	vocabulary = trainer.cluster();
	
	std::cout<<"--->Extracting BOW features ..."<<std::endl;
 	bowFeatures(images, vocabulary, conf.extractor);
 	
 	// Prepare traning data.
	Mat rawData;

	for(std::vector<BOWImg>::iterator iter = images.begin();iter != images.end(); iter++)
	{
		rawData.push_back(iter->BOWDescriptor);
		response.push_back(iter->label);
	}
	
	// PCA
#ifdef _USE_PCA_
	float factor = 1;
	int maxComponentsNum = static_cast<float>(conf.numClusters) * factor;
	PCA pca(rawData, Mat(),CV_PCA_DATA_AS_ROW, maxComponentsNum);
	Mat pcaData;
	for(int i = 0;i<rawData.rows;i++)
	{
		Mat vec = rawData.row(i);
		Mat coeffs = pca.project(vec);
		pcaData.push_back(coeffs);
	}	
	trainData = pcaData;
#else
	trainData = rawData;
#endif	
	return true;
}

// Train
void *train(Mat &trainData, Mat &response)
{
	if(conf.classifier == "SVM")
 	{
	 	std::cout<<"--->SVM Training ..."<<std::endl;	
		CvSVMParams params;
		params.kernel_type = CvSVM::LINEAR;
		params.svm_type = CvSVM::C_SVC;
		params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,1000,1e-6);
		
		CvSVM *classifier = new CvSVM;
		classifier->train(trainData,response,Mat(),Mat(),params);

		int c = classifier->get_support_vector_count();
	 	printf("    %d support vectors founded.\n",c);
	 	return (void *)classifier;
	}
	else if(conf.classifier == "BP")
	{
		std::cout<<"--->BP Training ..."<<std::endl;
		
	  	// Data transforming.
		int numClass = (int)conf.classes.size();
	   	cv::Mat labelMat = Mat::zeros(response.rows, numClass, CV_32F);
		for(int i = 0;i<response.rows;i++)
		{
			int k = response.ptr<int>()[i];
			labelMat.ptr<float>(i)[k-1] = 1.0;
		}
			
		// Set up BP network parameters
		CvANN_MLP_TrainParams params;
		params.term_crit = cvTermCriteria( CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 1000, 0.01 ),
		params.train_method = CvANN_MLP_TrainParams::BACKPROP;  
		params.bp_dw_scale = 0.1;  
		params.bp_moment_scale = 0.1;  
		
	   	CvANN_MLP *classifier = new CvANN_MLP;
	 
	   	// Set up the topology structure of BP network.
	   	int inputNodeNum = trainData.cols;
		float ratio = 2;
	   	int hiddenNodeNum = static_cast<float>(inputNodeNum) * ratio;
	   	int outputNodeNum = numClass;
	   	int maxHiddenLayersNum = 1;
		
	   	// It could be better! esp. Output node amount
	   	printf("    %d input nodes, %d output nodes.\n",inputNodeNum, outputNodeNum);
	   	Mat layerSizes;
	   	layerSizes.push_back(inputNodeNum);
	   	printf("    Hidden layers nodes' amount:");
		layerSizes.push_back(hiddenNodeNum);
		
	   	for(int i = 0 ;i<maxHiddenLayersNum;i++)
	   	{
	   		if(hiddenNodeNum > outputNodeNum * ratio + 1 )
			{
				hiddenNodeNum = static_cast<float>(hiddenNodeNum) / ratio;
				layerSizes.push_back(hiddenNodeNum);
				printf(" %d ",hiddenNodeNum);
			}
			else 
				break;
	   	}
	   	printf("\n");
	   	layerSizes.push_back(outputNodeNum);
	   	
	   	classifier->create(layerSizes, CvANN_MLP::SIGMOID_SYM);
		classifier->train(trainData, labelMat, Mat(),Mat(), params); 
		return (void *)classifier;
	}
	else{
		std::cout<<"--->Error: wrong classifier."<<std::endl;
		return NULL;
	}	
}

