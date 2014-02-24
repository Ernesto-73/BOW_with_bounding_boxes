/* 	
	\file: conf.cpp
	\author: Javier Zhang 
	\date: 2-24-2014
*/

#include "conf.h"

extern Config conf;

static struct termios oldt;

static void restore_terminal_settings(void)
{
    tcsetattr(0, TCSANOW, &oldt); 
}

static void disable_terminal_return(void)
{
    struct termios newt;
    tcgetattr(0, &oldt); 
    newt = oldt;  
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &newt);
    atexit(restore_terminal_settings);
}

bool configuration(int argc, char *argv[])
{
	Cxml parser;
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
			return false;
		}
		
		//conf.path = "/home/javier/Caltech-101/";
	
		// Default options
		conf.detector = "SIFT";	// detector
		conf.extractor = "SIFT";	// extractor
		conf.classifier = "SVM";
		conf.cluster = "Kmeans++";
		conf.numClusters = 50;
	
		std::string tmp("50");
		
		// Parse options
		for(int i=1;i<argc; i+=2)
		{
			if(i+1 == argc )
				break;
			else {
				if(std::string(argv[i]) == "-p") {
					conf.path = std::string(argv[i+1]);
				}else if(std::string(argv[i]) == "-e") {
					conf.extractor = std::string(argv[i+1]);
				}else if(std::string(argv[i]) == "-c") {
					conf.classifier = std::string(argv[i+1]);
				}else if(std::string(argv[i]) == "-C") {
					conf.cluster = std::string(argv[i+1]);
				}else if(std::string(argv[i]) == "-n") {
					tmp = argv[i+1];
					conf.numClusters = atoi(tmp.c_str());
				}else {
					std::cerr<<"ERROR: '"<<argv[i]<<"' is invalid arguments, please try again."<<std::endl; 
					return false;
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
		std::cout<<"Configuration file 'bow.xml' has been created."<<std::endl;
	}
	
	conf.max_num = 50; // At most read max_num pictures in every class. 
	
	std::cout<<"--->Training path: "<<conf.path<<std::endl;
	std::cout<<"--->Feature detector: "<<conf.detector<<std::endl;
	std::cout<<"--->Feature extractor: "<<conf.extractor<<std::endl;
	std::cout<<"--->Classifier: "<<conf.classifier<<std::endl;
	std::cout<<"--->Cluster algorithm: "<<conf.cluster<<std::endl;
	std::cout<<"--->Cluster medoids amount: "<<conf.numClusters<<std::endl;
	
	// Read clsval to conf struct
	std::fstream fs;
	std::string tmp = conf.path+"/clsval";
	fs.open(tmp.c_str(),std::fstream::in);
	char buf[256];
	while(fs.getline(buf,256))
	{
		std::string tmp = trim(std::string(buf));
		if(!tmp.empty() && tmp[0] != '#')	// use '#' to comment if U want
			conf.classes.push_back(tmp);
	}
	fs.close();
	
	printf("--->Selected categories [%d]: ",(int)conf.classes.size());
	unsigned k;
	for(k=0;k<conf.classes.size()-1;k++)
		printf("<%s>, ",conf.classes[k].c_str());
	printf("<%s>\n",conf.classes[k].c_str());
	
	std::cout<<"\nDo you want to continue [Y/n]? "<<std::endl;
	
	disable_terminal_return();
	char key = getchar();
	if(key == 'n' || key == 'N')
		return	false;
	return true;
}

std::string trim(std::string s, const char* t)
{
	s.erase(0, s.find_first_not_of(t));
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

