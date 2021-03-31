#ifndef __PRINTER_H__
#define __PRINTER_H__

#include <string>
#include <fstream>

#include <gis/logger.h>
#include <gis/gis.h>

class Printer
{
    std::string _buffer;

public:
    Printer();
    ~Printer(void);

    int add(char const* ptr);

    friend std::ofstream& operator<< (std::ofstream& out, Printer &value);
};

#endif /* __PRINTER_H__ */

