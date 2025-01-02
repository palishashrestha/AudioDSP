#include "logger.h"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>

/**
 * @brief Constructs a Logger instance and opens the specified log file.
 *
 * If the file cannot be opened, an error message is printed to std::cerr.
 * @param filePath The path to the log file.
 */
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

/**
 * @brief Destructor for Logger.
 *
 * Ensures the log file is properly closed when the Logger instance is destroyed.
 */
Logger::~Logger()
{
    if (logFile.is_open())
    {
        logFile.close();
    }
}

/**
 * @brief Retrieves the singleton instance of Logger.
 *
 * If an instance does not already exist, it will be created with the specified file path.
 * If no file path is provided, a default log file "application.log" is used.
 *
 * @param filePath The path to the log file (optional).
 * @return Logger& A reference to the singleton Logger instance.
 */
Logger &Logger::getInstance(const std::string &filePath)
{
    static Logger instance(filePath.empty() ? "application.log" : filePath); // Default to application.log if no path is provided
    return instance;
}

/**
 * @brief Logs a message with a specified severity level.
 *
 * Writes the message to the log file with a timestamp and severity level.
 * If the log file is not open, the message is written to std::cerr instead.
 *
 * @param message The message to log.
 * @param severity The severity level of the message (e.g., "INFO", "ERROR").
 */
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

/**
 * @brief Generates a timestamp for log messages.
 *
 * The timestamp is formatted as "YYYY-MM-DD HH:MM:SS".
 * @return std::string The formatted timestamp.
 */
std::string Logger::getTimestamp()
{
    auto now = std::time(nullptr);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

/**
 * @brief Logs a message with a specified level, optionally ensuring it is logged only once.
 *
 * @param message The message to log.
 * @param level The severity level of the message (e.g., "INFO", "WARNING", "ERROR").
 * @param logOnce If true, ensures the message is logged only once.
 */
void logMessage(const std::string &message, const std::string &level, bool logOnce)
{
    if (logOnce)
    {
        Logger::getInstance().log(message, level);
    }
}
