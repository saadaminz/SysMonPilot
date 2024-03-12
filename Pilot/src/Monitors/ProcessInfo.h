#ifndef _PROCESSINFO_H
#define _PROCESSINFO_H

class ProcessInfo
{
public:
	typedef std::vector<ProcessInfo> Vector;

	unsigned int pid;
	unsigned int ppid;
	unsigned int sid;
	unsigned int memoryMb;
	unsigned int threads;
	unsigned int handles;
	std::string name;
	std::string owner;
	std::string description;
	double cpu;

	ProcessInfo() : pid(0), ppid(0), sid(0), cpu(0), memoryMb(0), threads(0), handles(0), name(""), owner(""), description("") {}
	void PrintInfo() const;
};

#endif