#include <Poco/Logger.h>

#include "ProcessInfo.h"

void ProcessInfo::PrintInfo() const {
	Poco::Logger& logger = Poco::Logger::get("ProcessInfo");

	logger.information("Name: %s, Owner: %s, Description: %s, PID: %u, PPID: %u, SID: %u, Threads: %u, Handles: %u",
		name, owner, description, pid, ppid, sid, threads, handles);
}