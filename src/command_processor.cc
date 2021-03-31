#include "command_processor.h"
#include "trim.h"


CommandProcessor::CommandProcessor(Gis const* const gis)
{
    _gis = gis;
    _log = Logger::get_logger();
    _out = new Printer();
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

    *_log << buffer << std::endl;

    if (buffer == "world")
    {
        if (_out)
        {
            _out->add("printstr");
        }
    }

exit:
    return rc;
}

char* operator>> (char *const in, CommandProcessor &value)
{
    int rc = value._kernel(trim(std::string(in)));
    if (rc)
    {
        *value._log << ERROR << "CommandProcessor::_kernel() return ";
        *value._log << rc << " code" << std::endl;
    }

    return in;
}

std::ifstream& operator>> (std::ifstream& in, CommandProcessor &value)
{
    std::string temp;
    in >> temp;

    int rc = value._kernel(temp);
    if (rc)
    {
        *value._log << ERROR << "CommandProcessor::_kernel() return ";
        *value._log << rc << " code" << std::endl;
    }

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
