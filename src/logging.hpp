/*

***********************************************************
* © Benoit Leveau, inc. All rights reserved. *
***********************************************************

*/

#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <string>
#include <chrono>

enum LoggingLevel {
  ERROR    = 1,
  WARNING  = 2,
  INFO     = 3,
  DEBUG    = 4
};

std::string get_memory_display(size_t memory, bool show_gb=false);

class Logger
{
public:
    Logger(int verbosity);

    void log(std::string message, LoggingLevel level);

    void log_current_time(std::string message, LoggingLevel level);
    void log_system_info(LoggingLevel level);

private:
    std::chrono::time_point<std::chrono::steady_clock> start_time;
    int verbosity;
};

// TODO:ProgressLog should inherit from Logger, and it should override log() to add an indentation
// so we can cascade ProgressLogs and also add messages that are not just percent updates
class ProgressLog
{
public:
    ProgressLog(std::string operation, Logger &logger);
    ~ProgressLog();

    void update(float progress);
    void done();

private:
    Logger &logger;
    std::string operation;
    float last_displayed_progress;
    bool is_done;
    std::chrono::time_point<std::chrono::steady_clock> start_time;
    std::chrono::time_point<std::chrono::steady_clock> last_displayed_time;
};

#endif // !LOGGING_HPP
