#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>

class Logger
{
public:
    static Logger &getInstance(const std::string &filePath = "application.log"); // Default log file path

    void log(const std::string &message, const std::string &severity = "INFO"); // Add severity to log message

private:
    Logger(const std::string &filePath);        // Constructor with log file path
    ~Logger();                                  // Private destructor
    Logger(const Logger &) = delete;            // Disable copy constructor to implement Singleton design pattern
    Logger &operator=(const Logger &) = delete; // Disable assignment operator

    std::ofstream logFile; // Log file stream
    std::mutex logMutex;   // Mutex to ensure thread safety

    std::string getTimestamp(); // Helper function to get timestamp
};

#endif
