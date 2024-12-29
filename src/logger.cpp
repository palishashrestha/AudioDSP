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
    else
    {
        std::cout << "Log file opened successfully: " << filePath << std::endl;
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

// Log message with severity
void Logger::log(const std::string &message, const std::string &severity)
{
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile.is_open())
    {
        logFile << getTimestamp() << " [" << severity << "] - " << message << std::endl;
        logFile.flush(); // Ensure the log is written immediately
    }
    else
    {
        std::cerr << getTimestamp() << " [" << severity << "] - Log file is not open. Message: " << message << std::endl;
    }
}

// Get timestamp for log messages
std::string Logger::getTimestamp()
{
    auto now = std::time(nullptr);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// Helper function to log messages conditionally
void logMessage(const std::string &message, const std::string &level, bool logOnce)
{
    if (logOnce)
    {
        Logger::getInstance().log(message, level);
    }
}
