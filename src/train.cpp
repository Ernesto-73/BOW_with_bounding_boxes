#include "cxml.h"
#include "readdata.h"
#include <fstream>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>

using namespace std;
using namespace cv;

Config conf;

void features(std::vector<BOWImg> &images, string ext, string det)
{	
   	cv::initModule_nonfree();
  	Ptr<FeatureDetector> detector = FeatureDetector::create(det);
	Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create(ext);
	for(std::vector<BOWImg>::iterator iter = images.begin();iter != images.end(); iter++)
	{
		cv::Mat img = Mat(iter->img,Rect(iter->box[2],iter->box[0],iter->box[3]-iter->box[2],iter->box[1]-iter->box[0]));
		detector->detect(img,iter->keyPoints);
		extractor->compute(img,iter->keyPoints,iter->descriptor);
	}
}

void bowFeatures(std::vector<BOWImg> &images, cv::Mat vocabulary, string ext)
{
	cv::initModule_nonfree();
	Ptr<DescriptorExtractor> extractor=DescriptorExtractor::create(ext);
	Ptr<DescriptorMatcher> matcher=DescriptorMatcher::create("BruteForce");

	BOWImgDescriptorExtractor bow(extractor,matcher);
	bow.setVocabulary(vocabulary);

	for(std::vector<BOWImg>::iterator iter = images.begin();iter != images.end(); iter++)
	{
		bow.compute(iter->img,iter->keyPoints,iter->BOWDescriptor);
		
		// To Do: Need normalization or not ?
		normalize(iter->BOWDescriptor,iter->BOWDescriptor,1.0,0.0,NORM_MINMAX);
	}
}

void test(Mat &vocabulary, void *src,int type)
{
	// Test	
	std::cout<<"--->Testing ..."<<std::endl;
	std::vector<BOWImg> images;
	conf.max_num = conf.max_num * 2;
	int numImages = imgRead(images);
	if(numImages < 0)
		return ;
	
	features(images, conf.extractor, conf.detector);
	bowFeatures(images, vocabulary, conf.extractor);
	
	Mat testData;
	for(std::vector<BOWImg>::iterator iter = images.begin();iter != images.end(); iter++)
		testData.push_back(iter->BOWDescriptor);
	
	Mat output;
	if(!type)
	{
		CvANN_MLP *classifier = (CvANN_MLP *)src;
		classifier->predict(testData,output);
	}
	else {
		CvSVM *classifier = (CvSVM *)src;
		classifier->predict(testData,output);
	}
	
	cout<<"--->Predict answer: "<<std::endl;
	for(unsigned i=0;i<images.size();i++)
		std::cout<<"    "<<images[i].imgName<<" ---- "<<conf.classes[(unsigned)output.ptr<float>()[i]-1]<<endl;
}
 
int main(int argc, char *argv[])
{
	// BOW configurations.
	Cxml parser;
	printf("\n");	
	if(parser.parse("./bow.xml"))
	{
		std::cout<<"--->Using configuration file 'bow.xml'."<<std::endl;
		conf.path = trim(parser.getpNode()->eval_to_string(Glib::ustring("/BOW_Configurations/path")));
		conf.extractor = trim(parser.getpNode()->eval_to_string(Glib::ustring("/BOW_Configurations/extractor")));
		conf.detector = trim(parser.getpNode()->eval_to_string(Glib::ustring("/BOW_Configurations/detector")));
		conf.cluster = trim(parser.getpNode()->eval_to_string(Glib::ustring("/BOW_Configurations/cluster")));
		conf.classifier = trim(parser.getpNode()->eval_to_string(Glib::ustring("/BOW_Configurations/classifier")));
		conf.numClusters = parser.getpNode()->eval_to_number(Glib::ustring("/BOW_Configurations/numClusters"));
	}
	else {
		// config with args
		std::cout<<"--->Using command line args."<<std::endl;
		if(argc < 3)
		{
			std::cout<<std::endl
					 <<"Usage:"<<argv[0]<< " [-p] [-e*] [-c*] [-C*] [-n*]"<<std::endl
					 <<"		-p Training Path. "<<std::endl
					 <<"		-e Extractor. { SIFT| SURF| Harris }."<<std::endl
					 <<"		-c Classifier. { SVM| GMM| BP }"<<std::endl
					 <<"		-C Cluster. { Kmeans| Kmeans++| GMM }"<<std::endl
					 <<"		-n Amount of clusters. { 50 }"<<std::endl
					 <<std::endl;
			return EXIT_FAILURE;
		}
		
		//conf.path = "/home/javier/Caltech-101/";
	
		// Default options
		conf.detector = "SIFT";	// detector
		conf.extractor = "SIFT";	// extractor
		conf.classifier = "SVM";
		conf.cluster = "Kmeans++";
		conf.numClusters = 50;
	
		string tmp("50");
		
		// Parse options
		for(int i=1;i<argc; i+=2)
		{
			if(i+1 == argc )
				break;
			else {
				if(string(argv[i]) == "-p") {
					conf.path =string(argv[i+1]);
				}else if(string(argv[i]) == "-e") {
					conf.extractor = string(argv[i+1]);
				}else if(string(argv[i]) == "-c") {
					conf.classifier = string(argv[i+1]);
				}else if(string(argv[i]) == "-C") {
					conf.cluster = string(argv[i+1]);
				}else if(string(argv[i]) == "-n") {
					tmp = argv[i+1];
					conf.numClusters = atoi(tmp.c_str());
				}else {
					std::cerr<<"ERROR: '"<<argv[i]<<"' is invalid arguments, please try again.\n"; 
					return EXIT_FAILURE;
				}
			}
		}
		
		// Save configurations to a .xml file
		parser.addRoot("BOW_Configurations");
	
		parser.addItem("path",conf.path);
		parser.addItem("annotationsPath",conf.path+"/Annotations");
		parser.addItem("imagesPath",conf.path+"/Images");
		parser.addItem("extractor",conf.extractor);
		parser.addItem("detector",conf.detector);
		parser.addItem("classifier",conf.classifier);
		parser.addItem("cluster",conf.cluster);
		parser.addItem("numClusters",tmp);
	
		parser.create("./bow.xml");
	}

		std::cout<<"--->Training path: "<<conf.path<<std::endl;
		std::cout<<"--->Feature detector: "<<conf.detector<<std::endl;
		std::cout<<"--->Feature extractor: "<<conf.extractor<<std::endl;
		std::cout<<"--->Classifier: "<<conf.classifier<<std::endl;
		std::cout<<"--->Cluster algorithm: "<<conf.cluster<<std::endl;
		std::cout<<"--->Cluster medoids amount: "<<conf.numClusters<<std::endl;

	conf.max_num = 10; // At most read max_num pictures in every class. 
	
	// Read clsval to conf struct
	std::fstream fs;
	string tmp = conf.path+"/clsval";
	fs.open(tmp.c_str(),std::fstream::in);
	char buf[256];
	while(fs.getline(buf,256))
		conf.classes.push_back(trim(string(buf)));		
	fs.close();
	
	printf("--->Selected categories [%d]: ",(int)conf.classes.size());
	unsigned k;
	for(k=0;k<conf.classes.size()-1;k++)
		printf("<%s>, ",conf.classes[k].c_str());
	printf("<%s>\n",conf.classes[k].c_str());
			
	// Load training images
	std::cout<<"--->Loading images ... "<<std::endl;
	std::vector<BOWImg> images;
	int numImages = imgRead(images);
	if(numImages < 0)
		return EXIT_FAILURE;
	
	/*
		Get the ROI of images. 
		Feature detect and extract descriptors.
	*/
	printf("\n");
	printf("--->Extracting %s features ...\n", conf.extractor.c_str());	
	features(images, conf.extractor, conf.detector);
	
	BOWKMeansTrainer trainer(conf.numClusters,TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,10,1.0));
	for(std::vector<BOWImg>::iterator iter = images.begin();iter != images.end(); iter++)
	{
		Mat tmp = iter->descriptor;
		trainer.add(tmp);
	}
	
	std::cout<<"--->Constructing vocabulary list ..."<<std::endl;	
	Mat vocabulary = trainer.cluster();
	
	
	std::cout<<"--->Extracting BOW features ..."<<std::endl;
 	bowFeatures(images, vocabulary, conf.extractor);
 	
 	// Prepare traning data.
	Mat trainData;
	Mat response;
	
	for(std::vector<BOWImg>::iterator iter = images.begin();iter != images.end(); iter++)
	{
		trainData.push_back(iter->BOWDescriptor);
		response.push_back(iter->label);
	}
		
	// Train
 	if(conf.classifier == "SVM")
 	{
	 	std::cout<<"--->SVM Training ..."<<std::endl;	
		CvSVMParams params;
		params.kernel_type = CvSVM::LINEAR;
		params.svm_type = CvSVM::C_SVC;
		params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,1000,1e-6);

		CvSVM classifier;
		classifier.train(trainData,response,Mat(),Mat(),params);

		int c = classifier.get_support_vector_count();
	 	printf("    %d support vectors founded.\n",c);
	 	test(vocabulary, (void *)&classifier, 1);
	}
	else if(conf.classifier == "BP")
	{
		std::cout<<"--->BP Training ..."<<std::endl;	
	  	// Data transforming.
	   	cv::Mat labelMat;
		response.convertTo(labelMat,CV_32F);
	
		// Set up BP network parameters
		CvANN_MLP_TrainParams params;
		params.term_crit = cvTermCriteria( CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 1000, 0.01 ),
		params.train_method = CvANN_MLP_TrainParams::BACKPROP;  
		params.bp_dw_scale = 0.1;  
		params.bp_moment_scale = 0.1;  
		
	   	CvANN_MLP classifier;
	 
	   	// Set up the topology structure of BP network.
	   	int inputNodeNum = (int )conf.numClusters;
	   	int hiddenLayerNum = 10;
	   	int hiddenNodeNum = inputNodeNum;
	   	int outputNodeNum = 1;
	   	
	   	// It could be better! esp. Output node amount
	   	printf("    %d input nodes, %d hidden layers, %d output nodes.\n",inputNodeNum, hiddenLayerNum, outputNodeNum);
	   	Mat layerSizes;
	   	layerSizes.push_back(inputNodeNum);
	   	printf("    Hidden layers nodes' amount: ");
	   	for(int i=0;i<hiddenLayerNum;i++)
	   	{
	   		if(hiddenNodeNum > outputNodeNum * 2 + 1 )
				hiddenNodeNum = (int)(hiddenNodeNum / 2);
		   	layerSizes.push_back(hiddenNodeNum);
		   	printf(" %d ",hiddenNodeNum);
	   	}
	   	printf("\n");
	   	layerSizes.push_back(outputNodeNum);
	   	
	   	classifier.create(layerSizes, CvANN_MLP::SIGMOID_SYM);
		classifier.train(trainData, labelMat, Mat(),Mat(), params); 
		test(vocabulary, (void *)&classifier, 0);
	}
	else{
		std::cout<<"--->Error: wrong classifier."<<std::endl;
	}	
	return EXIT_SUCCESS;
}
