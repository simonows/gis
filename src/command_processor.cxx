#include "command_processor.h"


CommandProcessor::CommandProcessor(Gis const* const gis)
{
    _gis = gis;
    _log = Logger::get_logger();
}

CommandProcessor::~CommandProcessor()
{
}

int CommandProcessor::_kernel(std::string const &buffer)
{
    int rc = 0;

    if (!_out)
    {
        rc = EIO;
        goto exit;
    }



exit:
    return rc;
}

std::ifstream& operator>> (std::ifstream& in, CommandProcessor const &value)
{
    std::string temp;
    in >> temp;

    return in;
}

std::ofstream& operator<< (std::ofstream& out, CommandProcessor const &value)
{
    if (value._out)
    {
        out << *value._out;
    }
    return out;
}
