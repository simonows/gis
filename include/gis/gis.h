#ifndef __GIS_H__
#define __GIS_H__

#include <fstream>

#include <gis/logger.h>


typedef struct DMS
{
    char sign_lat;
    char sign_long;
    unsigned int degrees;
    unsigned int minutes;
    unsigned int seconds;
} DMS;

typedef struct GisRecord
{
    int feature_id;
    std::string feature_name;
    std::string feature_cat;
    std::string state;
    std::string country;
    DMS longtitude;
    DMS latitude;
    int elev;
    std::string usgs;
    int month, day, year;
} GisRecord;

class Gis
{
    Logger *log;
    DMS A[2], B[2];
    bool start_flag;

protected:

public:
    Gis()
    {
        log = Logger::get_logger();
        start_flag = false;
    }
    ~Gis() {}

    void set_bounds(DMS a_long, DMS a_lat, DMS b_long, DMS b_lat);
    bool get_flag();
};

#endif /* __GIS_H_ */
