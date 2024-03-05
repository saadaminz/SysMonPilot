#include <Poco/AutoPtr.h>
#include <Poco/Logger.h>
#include <Poco/SplitterChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/FileChannel.h>
#include <Poco/File.h>
#include <Poco/Path.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>

#include "Helpers/WmiWrapper.h"
#include "Monitors/SystemInfo.h"

void InitializeLogger() {
    
    // Create directory for storing logs
    std::string logFilePath = "C:\\ProgramData\\SysMonPilot\\Logs\\Pilot.log";
    Poco::Path path(logFilePath);
    Poco::File dir(path.parent());
    if (!dir.exists()) dir.createDirectories();

    // Create a formatter
    Poco::AutoPtr<Poco::PatternFormatter> pFormatter(new Poco::PatternFormatter);
    pFormatter->setProperty("pattern", "%Y-%m-%d %H:%M:%S %s: [%p] %t");
    pFormatter->setProperty("times", "UTC");

    // Create ConsoleChannel
    Poco::AutoPtr<Poco::ConsoleChannel> pCons(new Poco::ConsoleChannel);

    // Create FileChannel
    Poco::AutoPtr<Poco::FileChannel> pFile(new Poco::FileChannel);
    pFile->setProperty("path", logFilePath);
    pFile->setProperty("rotation", "10 M");
    pFile->setProperty("archive", "timestamp");
    pFile->setProperty("purgeCount", "1");

    // Wrap channels with formatter
    Poco::AutoPtr<Poco::FormattingChannel> pFCConsole(new Poco::FormattingChannel(pFormatter, pCons));
    Poco::AutoPtr<Poco::FormattingChannel> pFCFile(new Poco::FormattingChannel(pFormatter, pFile));

    // Create SplitterChannel and add both channels to it
    Poco::AutoPtr<Poco::SplitterChannel> pSplitter(new Poco::SplitterChannel);
    pSplitter->addChannel(pFCConsole);
    pSplitter->addChannel(pFCFile);

    // Assign the SplitterChannel to the root logger
    Poco::Logger& root = Poco::Logger::root();
    root.setChannel(pSplitter);
    root.setLevel(Poco::Message::PRIO_DEBUG);
}

int main() {
    InitializeLogger();
    SystemInfo si;
    WmiWrapper wmi;
    if (wmi.Initialize()) {
        wmi.GetSystemInfo(si);
        si.PrintInfo();
    }
    return 0;
}