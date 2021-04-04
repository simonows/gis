#include <errno.h>
#include <iostream>

#include <gis/logger.h>
#include "command_processor.h"

enum
{
    NUMBER_OF_ARGS = 3,
    MAX_STRING_LEN = 256,
};

int main(int argc, char **argv)
{
    int rc = 0;
    char buffer[MAX_STRING_LEN] = { 0 };
    std::ifstream script;
    std::ofstream outlog, output;
    Logger &log = *Logger::get_logger();
    Gis gis;
    CommandProcessor comproc(&gis);

    log << "GIS system started" << std::endl;

    if (argc < NUMBER_OF_ARGS)
    {
        rc = EINVAL;
        log << ERROR << "Wrong arguments (code = ";
        log << "Wrong arguments. (code = " << rc << ") ";
        log << "Try use to ./GIS <input> <gisfile> <output>";
        log << std::endl;
        goto exit;
    }

    log.set_input_file(std::string(argv[1]));
    script.open(argv[1], std::ios::in);
    outlog.open(argv[2], std::ios::out);

    while (!script.eof())
    {
        script.getline(buffer, MAX_STRING_LEN);
        buffer >> comproc;
        outlog << comproc;
    }

exit:
    return rc;
}

