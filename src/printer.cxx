#include "command_processor.h"
#include "printer.h"

Printer::Printer()
{
}

Printer::~Printer()
{
}

std::ofstream& operator<< (std::ofstream& out, Printer const &value)
{
    return out;
}
