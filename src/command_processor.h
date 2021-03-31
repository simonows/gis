#ifndef __COMMAND_PROCESSOR_H__
#define __COMMAND_PROCESSOR_H__

#include <string>
#include <fstream>

#include <gis/logger.h>
#include <gis/gis.h>

#include "printer.h"

class CommandProcessor
{
    Gis const* _gis;
    char const* _script_name;
    Printer *_out;
    Logger *_log;

protected:
    int _kernel(std::string const &buffer);

public:
    CommandProcessor(Gis const*);
    ~CommandProcessor(void);

    friend std::ifstream& operator>> (std::ifstream& in, CommandProcessor const &value);
    friend std::ofstream& operator<< (std::ofstream& out, CommandProcessor const &value);
};

#endif /* __COMMAND_PROCESSOR_H__ */

