#include "trim.h"
#include "command_processor.h"
#include "printer.h"

Printer::Printer(): _buffer()
{
}

Printer::~Printer()
{
}

int Printer::add(char const *const ptr)
{
    int rc = 0;
    _buffer += ptr;
    return rc;
}

int Printer::add(std::string const &ptr)
{
    int rc = 0;
    _buffer += ptr;
    return rc;
}

std::ofstream& operator<< (std::ofstream &out, Printer &value)
{
    trim(value._buffer);
    /*value._buffer.erase(
        std::remove(value._buffer.begin(), value._buffer.end(), '\n')
      , value._buffer.end()
    );*/

    if (value._buffer.size())
    {
        out << value._buffer << std::endl;
    }
    value._buffer.clear();

    return out;
}
