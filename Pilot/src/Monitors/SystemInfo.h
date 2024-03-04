#ifndef _SYSTEMINFO_H
#define _SYSTEMINFO_H

class SystemInfo
{
public:
	float disk_space;
	float disk_space_free;
	unsigned long ipv4_address;
	unsigned long memory_mb;

	unsigned int cpu_cores;
	unsigned int cpu_threads;
	unsigned int cpu_max_clock;
	std::string cpu_name;
	std::string cpu_architecture;

	std::string os_architecture;
	std::string os_version;
	std::string os_type;
	std::string os_name;

	std::string sys_user;
	std::string sys_hostname;
	std::string sys_domain;
	std::string sys_manufacturer;
	std::string sys_model;
	std::string sys_owner;
	std::string sys_family;

	SystemInfo() : memory_mb(0), disk_space(0.0), disk_space_free(0.0), ipv4_address(0),
		cpu_cores(0), cpu_threads(0), cpu_max_clock(0), cpu_name(""), os_name(""), cpu_architecture(""), os_architecture(""), os_version(""), os_type(""),
		sys_user(""), sys_hostname(""), sys_domain(""), sys_manufacturer(""), sys_model(""), sys_owner(""), sys_family("")
	{}

	void PrintInfo() const;
};

#endif
