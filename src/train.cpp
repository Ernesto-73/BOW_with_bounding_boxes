#include "cxml.h"
#include "readdata.h"
#include <fstream>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>

using namespace std;
using namespace cv;

Config conf;

void features(std::vector<BOWImg> &images, string ext, string des)
{	
   	cv::initModule_nonfree();
  	Ptr<FeatureDetector> detector = FeatureDetector::create(des);
	Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create(ext);
	for(std::vector<BOWImg>::iterator iter = images.begin();iter != images.end(); iter++)
	{
		cv::Mat img = Mat(iter->img,Rect(iter->box[2],iter->box[0],iter->box[3]-iter->box[2],iter->box[1]-iter->box[0]));
		/*
		cv::imshow("test",img);
		cv::waitKey(0);
		*/
		detector->detect(img,iter->keyPoints);
		extractor->compute(img,iter->keyPoints,iter->descriptor);
	}
}

void bowFeatures(std::vector<BOWImg> &images, cv::Mat vocabulary, string ext, Mat &trainData, Mat &response)
{
	cv::initModule_nonfree();
	Ptr<DescriptorExtractor> extractor=DescriptorExtractor::create(ext);
	Ptr<DescriptorMatcher> matcher=DescriptorMatcher::create("BruteForce");

	BOWImgDescriptorExtractor bow(extractor,matcher);
	bow.setVocabulary(vocabulary);

	for(std::vector<BOWImg>::iterator iter = images.begin();iter != images.end(); iter++)
	{
		bow.compute(iter->img,iter->keyPoints,iter->BOWDescriptor);
		//normalize(iter->BOWDescriptor,iter->BOWDescriptor,1.0,0.0,NORM_MINMAX);
		trainData.push_back(iter->BOWDescriptor);
		response.push_back(iter->label);
	}
}

int main(int argc, char *argv[])
{
	// BOW configurations.
	Cxml parser;
	if(parser.parse("./bow.xml"))
	{
		std::cout<<"Using configuration file."<<std::endl;
		conf.trainingPath = trim(parser.getpNode()->eval_to_string(Glib::ustring("/BOW_Configurations/trainingPath")));
		conf.extractor = trim(parser.getpNode()->eval_to_string(Glib::ustring("/BOW_Configurations/extractor")));
		conf.descriptor = trim(parser.getpNode()->eval_to_string(Glib::ustring("/BOW_Configurations/descriptor")));
		conf.cluster = trim(parser.getpNode()->eval_to_string(Glib::ustring("/BOW_Configurations/cluster")));
		conf.classifier = trim(parser.getpNode()->eval_to_string(Glib::ustring("/BOW_Configurations/classifier")));
		conf.numClusters = parser.getpNode()->eval_to_number(Glib::ustring("/BOW_Configurations/numClusters"));
	}
	else {
		// config with args
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
		
		//conf.trainingPath = "/home/javier/Caltech-101/";
	
		// Default options
		conf.descriptor = "SIFT";	// descriptor
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
					conf.trainingPath =string(argv[i+1]);
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
	
		parser.addItem("trainingPath",conf.trainingPath);
		parser.addItem("annotationsPath",conf.trainingPath+"/Annotations");
		parser.addItem("imagesPath",conf.trainingPath+"/Images");
		parser.addItem("extractor",conf.extractor);
		parser.addItem("descriptor",conf.descriptor);
		parser.addItem("classifier",conf.classifier);
		parser.addItem("cluster",conf.cluster);
		parser.addItem("numClusters",tmp);
	
		parser.create("./bow.xml");
	}
	
#ifdef DEBUG
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
		std::cout<<conf.trainingPath<<std::endl;
		std::cout<<conf.extractor<<std::endl;
		std::cout<<conf.descriptor<<std::endl;
		std::cout<<conf.classifier<<std::endl;
		std::cout<<conf.cluster<<std::endl;
		std::cout<<conf.numClusters<<std::endl;
#endif 

	conf.max_num = 10; // At most read max_num pictures in every class. 
	
	// Read clsval to conf struct
	std::fstream fs;
	string tmp = conf.trainingPath+"/clsval";
	fs.open(tmp.c_str(),std::fstream::in);
	char buf[256];
	while(fs.getline(buf,256))
	{
		std::cout<<buf<<std::endl;
		conf.classes.push_back(trim(string(buf)));		
	}
	fs.close();
	
	std::cout<<conf.classes.size()<<std::endl;
	
	// Load training images
	std::vector<BOWImg> images;
	int numImages = imgRead(images);
	if(numImages < 0)
		return EXIT_FAILURE;
	
	/*
		Get the ROI of images. 
		Feature detect and extract descriptors.
	*/	
	features(images, conf.extractor, conf.descriptor);
	
	BOWKMeansTrainer trainer(conf.numClusters,TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,10,1.0));
	for(std::vector<BOWImg>::iterator iter = images.begin();iter != images.end(); iter++)
	{
		Mat tmp = iter->descriptor;
		trainer.add(tmp);
	}
	
	Mat vocabulary = trainer.cluster();
	
	Mat trainData;
	Mat response;
 	bowFeatures(images, vocabulary, conf.extractor, trainData, response);
	
	CvSVMParams params;
	params.kernel_type=CvSVM::LINEAR;
	params.svm_type=CvSVM::C_SVC;
	params.term_crit=cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,1000,1e-6);

	CvSVM svm;
	svm.train(trainData,response,Mat(),Mat(),params);
	
	// save support vectors
	int c = svm.get_support_vector_count();
	for(int i = 0;i<c;i++)
	{
		const float* v = svm.get_support_vector(i);
		for(int j = 0;j<50;j++)
			printf("%f ",v[j]);
		printf("\n");
	}
	
	return EXIT_SUCCESS;
}
