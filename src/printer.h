#ifndef __PRINTER_H__
#define __PRINTER_H__

#include <string>
#include <fstream>

#include <gis/logger.h>
#include <gis/gis.h>

class Printer
{
public:
    Printer();
    ~Printer(void);

    friend std::ofstream& operator<< (std::ofstream& out, Printer const &value);
};

#endif /* __PRINTER_H__ */

