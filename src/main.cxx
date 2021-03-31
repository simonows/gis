#include <errno.h>
#include <iostream>

#include <gis/logger.h>
#include <gis/command_processor.h>


int main(int argc, char **argv)
{
    int rc = 0;
    Logger *log = Logger::get_logger();

    log << std::string("GIS system started");

    if (argc < 3)
    {
        log << ERROR << std::string("Wrong arguments");
        rc = EINVAL;
        goto exit;
    }

exit:
    return rc;
}

