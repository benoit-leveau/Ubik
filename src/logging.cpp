/*

***********************************************************
* © Benoit Leveau, inc. All rights reserved. *
***********************************************************

*/

#include <iostream>
#include <iomanip>
#include <sstream>

#include "logging.hpp"
#include "memory.hpp"


std::string get_memory_display(size_t memory)
{
    if (memory > 1024*1024*1024)
        return std::to_string(int(memory / (1024*1024*1024))) + "GB";
    else if (memory > 1024*1024)
        return std::to_string(int(memory / (1024*1024))) + "MB";
    else if (memory > 1024)
        return std::to_string(int(memory / 1024)) + "KB";
    return std::to_string(memory) + "B";
}

std::string get_level_message(LoggingLevel level)
{
    switch(level)
    {
    case DEBUG:
        return "DEBUG";
    case INFO:
        return "";
    case WARNING:
        return "WARNING";
    case ERROR:
        return "ERROR";
    }
}

ProgressLog::ProgressLog(std::string operation, Logger &logger) : logger(logger), operation(operation), is_done(false)
{
    start_time = std::chrono::steady_clock::now();
    last_displayed_time = std::chrono::steady_clock::now();
    last_displayed_progress = -5.0f;
    update(0.0f);
}


ProgressLog::~ProgressLog()
{
    if (!is_done)
        done();
}

void ProgressLog::update(float progress)
{
    auto current_time = std::chrono::steady_clock::now();
    double time = std::chrono::duration<double>(current_time-last_displayed_time).count();
    if ((progress == 100.0) || (progress - last_displayed_progress >= 5.0) || (time>30))
    {
        last_displayed_progress = progress;
        last_displayed_time = current_time;
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2) << std::setfill('0') << progress;
        std::string progress_message = operation + " " + stream.str() + "%";
        logger.log(progress_message, INFO);
    }
}

void ProgressLog::done()
{
    is_done = true;
    if (last_displayed_progress != 100.0)
        update(100.0);
    auto current_time = std::chrono::steady_clock::now();
    double time = std::chrono::duration<double>(current_time-start_time).count();
    std::string message = operation + " took " + std::to_string(time) + "s";
    logger.log(message, INFO);
}

Logger::Logger(int verbosity) : verbosity(verbosity)
{
    start_time = std::chrono::steady_clock::now();
}

void Logger::log(std::string message, LoggingLevel level)
{
    // check verbosity
    if (level > verbosity)
        return;

    std::string disp_message;

    // first get the time
    {
        auto t_now = std::chrono::steady_clock::now();
        int seconds = int(std::chrono::duration<double>(t_now-start_time).count());
        int hours = seconds/3600;
        seconds = seconds - hours * 3600;
        int minutes = seconds/60;
        seconds = seconds - minutes * 60;
        std::ostringstream stream;
        stream << std::setfill('0') << std::setw(2) << hours << ":" << std::setfill('0') << std::setw(2) << minutes << ":" << std::setfill('0') << std::setw(2) << seconds;
        disp_message += stream.str();
    }

    // then get the current RAM usage
    {
        std::ostringstream stream;
        size_t memory = getCurrentRSS();
        stream << std::setw(6) << std::right << get_memory_display(memory);
        disp_message += "  " + stream.str();
    }

    // then add LOG LEVEL: INFO, WARNING, ERROR
    {
        std::ostringstream stream;
        stream << std::setw(7) << get_level_message(level);
        disp_message += " " + stream.str();
    }

    // then add separator
    disp_message += " | ";

    // then add message
    disp_message += message;

    // display in console
    std::cout << disp_message << std::endl;
}
