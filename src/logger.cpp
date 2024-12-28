#include "logger.h"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>

Logger::Logger(const std::string &filePath)
{
    logFile.open(filePath, std::ios::app); // Log file in the specified path
    if (!logFile.is_open())
    {
        std::cerr << "Failed to open log file: " << filePath << std::endl;
    }
}

Logger::~Logger()
{
    if (logFile.is_open())
    {
        logFile.close();
    }
}

Logger &Logger::getInstance(const std::string &filePath)
{
    static Logger instance(filePath.empty() ? "application.log" : filePath); // Default to application.log if no path is provided
    return instance;
}

void Logger::log(const std::string &message, const std::string &severity)
{
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile.is_open())
    {
        logFile << getTimestamp() << " [" << severity << "] - " << message << std::endl;
        logFile.flush(); // Ensure data is written to disk immediately
    }
    else
    {
        std::cerr << getTimestamp() << " [" << severity << "] - Log file is not open. Message: " << message << std::endl;
    }
}

std::string Logger::getTimestamp()
{
    auto now = std::time(nullptr);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S"); // Format: YYYY-MM-DD HH:mm:ss
    return oss.str();
}

/// Helper function to handle conditional logging
void logMessage(const std::string &message, const std::string &level, bool logOnce)
{
    if (logOnce)
    {
        Logger::getInstance().log(message, level);
    }
}
