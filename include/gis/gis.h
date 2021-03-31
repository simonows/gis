#ifndef __GIS_H__
#define __GIS_H__

#include <fstream>

#include <gis/logger.h>

class Gis
{
    Logger *log;

protected:

public:
    Gis()
    {
        log = Logger::get_logger();
    }
    ~Gis() {}
};

#endif /* __GIS_H_ */
