#include <chrono>
#include <iostream>
#include <ctime>

#include <gis/logger.h>

#include "trim.h"

/**
 * Static methods should be defined outside the class.
 */
Logger* Logger::pinstance_{nullptr};
std::mutex Logger::mutex_;
std::string Logger::_input_file;

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

Logger& operator<< (Logger &out, std::string const value)
{
    out._buffer += value;
    return out;
}

Logger& operator<< (Logger &out, char const *const value)
{
    out._buffer += value;
    return out;
}

Logger& operator<< (Logger &out, LogCategory const &value)
{
    out.level = value;
    return out;
}

Logger& operator<< (Logger &out, int const &value)
{
    out._buffer += std::to_string(value);
    return out;
}

Logger& operator<< (Logger &out, std::ostream&(*param)(std::ostream&)
){
    auto end = std::chrono::system_clock::now();
    std::time_t end_time = 0;
    std::string now_time = "";

    if (param != static_cast<std::basic_ostream<char>&(*)(std::basic_ostream<char>&)>(std::endl))
    {
        goto exit;
    }

    end_time = std::chrono::system_clock::to_time_t(end);
    now_time = std::ctime(&end_time);

    *(out.stream) << now_time.substr(0, now_time.size() - 1) << ' ';

    switch (out.level)
    {
        case INFO:
            *(out.stream) << std::string("[INFO]") << "  ";
            break;
        case WARN:
            *(out.stream) << std::string("[WARN]") << "  ";
            break;
        case ERROR:
            *(out.stream) << std::string("[ERROR]") << ' ';
            break;
        case DEBUG:
            *(out.stream) << std::string("[DEBUG]") << ' ';
            break;
        default:
            break;
    }

    trim(out._buffer);
    *(out.stream) << out._buffer << std::endl;
    out._buffer.clear();

exit:
    return out;
}


std::string Logger::get_input_file(void)
{
    return _input_file;
}

void Logger::set_input_file(std::string input_file)
{
    _input_file = input_file;
}

