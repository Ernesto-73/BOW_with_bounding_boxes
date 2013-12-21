/* 	\file: SampleSet.h
	\author: Javier Zhang 
*/
#include "SampleSet.h"

/*
	the impletation of class SampleSet
*/
SampleSet::SampleSet(): usePrefix(false), start(0)
{
	num = 0;
}

SampleSet::SampleSet(bool _usePrefix, int _start): usePrefix(_usePrefix), start(_start)
{
	num = 0;
}

SampleSet::~SampleSet()
{
#ifdef DEBUG
	std::cout<<__FILE__<<__LINE__<<std::endl;
#endif 
}

void SampleSet::addDir(std::string _path, std::string _tag)
{
	if(access(_path.c_str(),R_OK | W_OK | F_OK))
	{
		std::string  mesg(strerror(errno));
		std::cout<<"Error("<<errno<<"): "<<mesg<<std::endl;
	}
	else {
		paths.push_back(_path);
		tags.push_back(_tag);
	}
}

void SampleSet::rename()
{
	int code = start;
	for(std::vector<std::string>::size_type i = 0; i < paths.size(); i++)
	{
		std::cout<<"Class Tag: "<<tags[i]<<std::endl;
		std::cout<<"Enter path '"<<paths[i]<<"'"<<std::endl;
		std::cout<<"Renaming files ... "<<std::endl;
		DIR *dir = opendir(paths[i].c_str());
		DStream ds;
		char buff[100];
		while((ds = readdir(dir)))
		{
			if(ds->d_type == DT_DIR)
				continue;
			std::string name(ds->d_name);
			std::string suffix = getSuffix(name);	// include '.'
			std::string cmd("mv ");
			cmd += paths[i] + "/" +std::string(name);
			sprintf(buff,"%06d",code++);
			std::string newName =  std::string(buff);
			
			if(usePrefix)
			{
				if(prefix.empty())
					newName = tags[i] + "_" + newName;
				else 
					newName = prefix + newName;
			}
			
			newName += suffix;
			cmd += " " + paths[i] + "/" + newName;
			if(name == newName)
				continue;
			system(cmd.c_str());
		}
	}
}

void SampleSet::createList(std::string listName)
{
	std::vector<std::string> list;
	for(std::vector<std::string>::size_type i = 0; i < paths.size(); i++)
	{
		std::cout<<"Enter path '"<<paths[i]<<"'"<<std::endl;
		std::cout<<"Recording files ... "<<std::endl;
		DIR *dir = opendir(paths[i].c_str());
		DStream ds;
		while((ds = readdir(dir)))
		{
			if(ds->d_type == DT_DIR)
				continue;
			std::string name(ds->d_name);
			//;
			list.push_back(stripSuffix(name));
		}
	}
	std::cout<<"Saving to file '"<<listName<<"' ... "<<std::endl;
	std::fstream fs;
	fs.open(listName.c_str(),std::fstream::out);
	for(std::vector<std::string>::iterator iter = list.begin();iter != list.end();iter++)
		fs<<*iter<<std::endl;
}

std::string SampleSet::num2str(int num)
{
	std::stringstream ss;
    ss<<num;
    return ss.str();
}

/*********************************/
/*		SETTERS && GETTERS		 */
/*********************************/
unsigned SampleSet::getTotalNum()
{
	return num;
}

void SampleSet::setPrefix(std::string _prefix)
{
	prefix = _prefix;
	return ;
}

std::string SampleSet::getSuffix(const std::string &name)
{
	int len = name.size();
	int pos, i;
	for(i=0;i<len;i++)
	{
		if(name[i] == '.')
			break;
	}
	pos = i;
	return name.substr(pos,len-pos);
}

void SampleSet::setUsePrefix(bool val)
{
	usePrefix = val;
}

std::string SampleSet::stripSuffix(const std::string &name)
{
	int len = name.size();
	std::string tmp;
	for(int i=0;i<len;i++)
	{	
		if(name[i] == '.')
			break;
		else 
			tmp += name[i];
	}
	return tmp;
}
