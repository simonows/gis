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
    std::ofstream outlog;
    Logger &log = *Logger::get_logger();
    auto end = std::chrono::system_clock::now();
    std::time_t end_time = 0;
    std::string now_time = "";
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
    gis.set_db_file(argv[1]);
    script.open(argv[2], std::ios::in);
    outlog.open(argv[3], std::ios::out);

    end_time = std::chrono::system_clock::to_time_t(end);
    now_time = std::ctime(&end_time);

    outlog << "Course Project for COMP 8042" << std::endl;
    outlog << "Student Name: Frederick Wong, Student Id: A01004384" << std::endl;
    outlog << "Begin of GIS Program log" << std::endl;
    outlog << "dbFile: " << argv[1] << std::endl;
    outlog << "script: " << argv[2] << std::endl;
    outlog << "log:    " << argv[3] << std::endl;
    outlog << "Start Time: " << now_time.substr(0, now_time.size() - 1) << std::endl;

    while (!script.eof() && comproc.get_state())
    {
        script.getline(buffer, MAX_STRING_LEN);
        buffer >> comproc;
        outlog << comproc;
    }

    script.close();
    outlog.close();

exit:
    return rc;
}

