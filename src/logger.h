#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>

class Logger
{
public:
    static Logger &getInstance();

    void log(const std::string &message);

private:
    Logger();                                   // Private constructor
    ~Logger();                                  // Private destructor
    Logger(const Logger &) = delete;            // Disable copy constructor to implement Singleton Design Pattern
    Logger &operator=(const Logger &) = delete; // Disable assignment operator to implement Singleton Design Pattern

    std::ofstream logFile;
    std::mutex logMutex;
};

#endif