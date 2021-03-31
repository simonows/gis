#include <errno.h>
#include <iostream>

#include <gis/logger.h>
#include "command_processor.h"


int main(int argc, char **argv)
{
    int rc = 0;
    std::ifstream script;
    std::ofstream outlog, output;
    Logger *log = Logger::get_logger();
    Gis gis;
    CommandProcessor comproc(&gis);

    log << std::string("GIS system started");

    if (argc < 3)
    {
        log << ERROR << std::string("Wrong arguments");
        rc = EINVAL;
        goto exit;
    }

    while (!script.eof())
    {
        script >> comproc;
        outlog << comproc;
    }

exit:
    return rc;
}

