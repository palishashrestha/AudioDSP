#include "logger.h"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

Logger::Logger()
{
    logFile.open("application.log", std::ios::app); // Log file in the current working directory
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
        logFile << getTimestamp() << " - " << message << std::endl; // Include timestamp
    }
    else
    {
        std::cerr << getTimestamp() << " - Log file is not open. Message: " << message << std::endl;
    }
}

std::string Logger::getTimestamp()
{
    auto now = std::time(nullptr);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S"); // Format: YYYY-MM-DD HH:mm:ss
    return oss.str();
}
