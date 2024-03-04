#include <iostream>

#include "SystemInfo.h"

void SystemInfo::PrintInfo() const {

	std::cout << "\nProcessorInfo:" << std::endl;
	std::cout << "CPU Name: " << cpu_name << std::endl;
	std::cout << "CPU Cores: " << cpu_cores << std::endl;
	std::cout << "CPU Threads: " << cpu_threads << std::endl;
	std::cout << "CPU Max Clock: " << cpu_max_clock << std::endl;
	std::cout << "CPU Architecture: " << cpu_architecture << std::endl;

	std::cout << "\nOperatingSystemInfo:" << std::endl;
	std::cout << "OS Name: " << os_name << std::endl;
	std::cout << "OS Architecture: " << os_architecture << std::endl;
	std::cout << "OS Version: " << os_version << std::endl;
	std::cout << "OS Type: " << os_type << std::endl;

	std::cout << "\nComputerSystemInfo:" << std::endl;
	std::cout << "System MemoryMB: " << memory_mb << std::endl;
	std::cout << "System User: " << sys_user << std::endl;
	std::cout << "System Hostname: " << sys_hostname << std::endl;
	std::cout << "System Domain: " << sys_domain << std::endl;
	std::cout << "System Manufacturer: " << sys_manufacturer << std::endl;
	std::cout << "System Model: " << sys_model << std::endl;
	std::cout << "System Owner: " << sys_owner << std::endl;
	std::cout << "System Family: " << sys_family << std::endl;

}
