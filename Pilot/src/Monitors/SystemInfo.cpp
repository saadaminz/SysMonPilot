#include <Poco/Logger.h>

#include "SystemInfo.h"

void SystemInfo::PrintInfo() const {
	Poco::Logger& logger = Poco::Logger::get("SystemInfo");

	logger.information("CPU Info - Name: %s, Cores: %u, Threads: %u, MaxClock: %u, Architecture: %s",
		cpu_name, cpu_cores, cpu_threads, cpu_max_clock, cpu_architecture);

	logger.information("OS Info - Name: %s, Architecture: %s, Version %s, Type: %s",
		os_name, os_architecture, os_version, os_type);

	logger.information("System Info - MemoryMB: %lu, User: %s, Hostname: %s, Domain: %s, Manufacturer: %s, Model: %s, Owner: %s, Family: %s",
		memory_mb, sys_user, sys_hostname, sys_domain, sys_manufacturer, sys_model, sys_owner, sys_family);
}
