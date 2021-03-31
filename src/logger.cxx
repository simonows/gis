#ifndef __LOGGER_CXX__
#define __LOGGER_CXX__

#include <chrono>
#include <iostream>
#include <ctime>

#include <gis/logger.h>

/**
 * Static methods should be defined outside the class.
 */
Logger* Logger::pinstance_{nullptr};
std::mutex Logger::mutex_;

/**
 * The first time we call GetInstance we will lock the storage location
 * and then we make sure again that the variable is null and then we
 * set the value
 */
Logger *Logger::get_logger(std::string const &path)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (pinstance_ == nullptr)
    {
        if (path == "")
        {
            pinstance_ = new Logger(std::cout);
        }
        else
        {
            pinstance_ = new Logger(path);
        }
    }
    return pinstance_;
}

Logger* operator<< (Logger *out, std::string const &value)
{
    auto end = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::string now_time = std::ctime(&end_time);

    *(out->stream) << now_time.substr(0, now_time.size() - 1) << ' ';

    switch (out->level)
    {
        case INFO:
            *(out->stream) << std::string("[INFO]") << "  ";
            break;
        case WARN:
            *(out->stream) << std::string("[WARN]") << "  ";
            break;
        case ERROR:
            *(out->stream) << std::string("[ERROR]") << ' ';
            break;
        case DEBUG:
            *(out->stream) << std::string("[DEBUG]") << ' ';
            break;
        default:
            break;
    }

    *(out->stream) << value << std::endl;
    return out;
}

Logger* operator<< (Logger *out, LogCategory const &value)
{
    out->level = value;
    return out;
}


#endif // __LOGGER_CXX__

