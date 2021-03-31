#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <string>
#include <mutex>
#include <fstream>

typedef enum LogCategory
{
    INFO = 0,
    WARN,
    ERROR,
    DEBUG
} LogCategory;

/**
 * The Lone class provides the `getInstance ' method, which behaves like an
 * alternative constructor and allows clients to get the same instance of the
 * class every time it is called.
*/
class Logger: public std::ofstream
{
private:
    static Logger *pinstance_;
    static std::mutex mutex_;
    LogCategory level;
    std::ofstream * stream;

protected:
    /**
     * Singleton's constructor should always be hidden to prevent
     * creating an object using the new operator.
    */
    Logger(std::string const &path): log_path(path), std::ofstream()
    {
        level = INFO;
        stream = this;
        open(path);
    }
    Logger(std::ostream &str)
    {
        level = INFO;
        stream = (std::ofstream *)&str;
    }
    ~Logger()
    {
        close();
    }

    std::string log_path;

public:
    /**
     * Singletons should not be cloned.
    */
    Logger(Logger &other) = delete;

    /**
     * Singletons should not be assignable.
    */
    void operator=(const Logger &) = delete;

    /**
     * This is a static method that controls access to a single instance. The
     * first time it runs, it creates a single instance and puts it in a static
     * field. On subsequent runs, it returns the object stored in the static
     * field to the client.
    */
    static Logger *get_logger(std::string const &path = "");

    void SomeBusinessLogic()
    {
        // ...
    }

    friend Logger* operator<< (Logger *out, std::string const &value);
    friend Logger* operator<< (Logger *out, LogCategory const &value);
};

#endif // __LOGGER_H__
