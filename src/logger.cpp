#include "logger.h"
#include <iostream>
#include <mutex>

Logger::Logger()
{
    logFile.open("application.log", std::ios::app);
    if (!logFile.is_open())
    {
        std::cerr << "Failed to open log file." << std::endl;
    }
}

Logger::~Logger()
{
    if (logFile.is_open())
    {
        logFile.close();
    }
}

Logger &Logger::getInstance()
{
    static Logger instance;
    return instance;
}

void Logger::log(const std::string &message)
{
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile.is_open())
    {
        logFile << message << std::endl;
    }
    else
    {
        std::cerr << "Log file is not open. Message: " << message << std::endl;
    }
}
