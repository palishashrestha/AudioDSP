#include "../logger.h"
#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <filesystem>

// Test case to verify logging functionality
TEST(LoggerTest, LogMessage)
{
    std::string testLogFile = "application.log"; // Test log file path
    Logger &logger = Logger::getInstance(testLogFile);
    logger.log("Unit Testing for Logger module", "INFO"); // Log a test message

    // Verify that the log file exists
    ASSERT_TRUE(std::filesystem::exists(testLogFile)) << "Log file was not created: " << testLogFile;

    // Open the log file and verify the content
    std::ifstream logFile(testLogFile);
    ASSERT_TRUE(logFile.is_open()) << "Failed to open the log file.";

    std::string line;
    bool messageLogged = false;
    while (std::getline(logFile, line))
    {
        if (line.find("Test message") != std::string::npos)
        {
            messageLogged = true;
            break;
        }
    }
    ASSERT_TRUE(messageLogged) << "Log message not found in the log file.";

    logFile.close();
    std::filesystem::remove_all("logs");
}

// Test case to verify singleton behavior
TEST(LoggerTest, SingletonBehavior)
{
    Logger &logger1 = Logger::getInstance("logs/test1.log");
    Logger &logger2 = Logger::getInstance("logs/test2.log");

    // Verify both instances are the same
    ASSERT_EQ(&logger1, &logger2) << "Logger instances are not the same.";
}
