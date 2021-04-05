#ifndef __GIS_H__
#define __GIS_H__

#include <fstream>
#include <vector>
#include <map>
#include <cstring>

#include <gis/logger.h>

#define POOL_SIZE 15

typedef struct DMS
{
    char sign_lat;
    char sign_long;
    unsigned int degrees;
    unsigned int minutes;
    unsigned int seconds;
} DMS;


long inline make_second(DMS const &arg)
{
    return (arg.sign_lat || arg.sign_long ? -1 : 1)
         * (arg.degrees * 3600 + arg.minutes * 60 + arg.seconds);
}

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
    long row;
} GisRecord;

class Gis
{
    Logger *log;
    DMS A[2], B[2];
    bool start_flag;
    std::string _db;
    std::fstream _data;
    std::vector<struct GisRecord>pool;
    std::map<std::string, unsigned long> hash_table;

protected:

public:
    Gis()
    {
        log = Logger::get_logger();
        start_flag = false;
    }
    ~Gis()
    {
        _data.close();
    }

    void set_bounds(DMS a_long, DMS a_lat, DMS b_long, DMS b_lat);
    bool get_flag();
    void set_db_file(char const *arg);
    int open();
    void add(char *);
    struct GisRecord *get(std::string name, std::string state);
};

#endif /* __GIS_H_ */
