#ifndef __GIS_H__
#define __GIS_H__

#include <fstream>
#include <vector>
#include <cstring>

#include <gis/logger.h>
#include <gis/coord.h>
#include <gis/hash.h>

#define POOL_SIZE 15


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

typedef struct GisRecordWithHash
{
    struct GisRecord rec;
    unsigned long hash;
} GisRecordWithHash;


class Gis
{
    Logger *log;
    DMS A[2], B[2];
    bool start_flag;
    std::string _db;
    std::fstream _data;
    std::vector<struct GisRecord>pool;
    HashMap<std::string, long> hash_table;
    HashMap<std::string, long> name_table;
    Coord quad;
    bool is_opened;

    size_t features_by_name_count;
    size_t longest_probe_count;
    size_t imported_location_count;
    size_t avg_name_length;

protected:

public:
    Gis()
    {
        log = Logger::get_logger();
        start_flag = false;
        features_by_name_count = 0;
        longest_probe_count = 0;
        imported_location_count = 0;
        avg_name_length = 0;
    }
    ~Gis()
    {
        _data.close();
    }

    inline std::vector<struct GisRecord> *get_pool() { return &pool; }
    void get_hash(std::vector<struct GisRecordWithHash> &mas);
    void set_bounds(DMS a_long, DMS a_lat, DMS b_long, DMS b_lat);
    bool get_flag();
    void set_db_file(char const *arg);
    int open();
    void add(char *);
    struct GisRecord *get(std::string name, std::string state);
    int get(
        std::vector<struct GisRecord *> &mas
      , DMS const &a_long
      , DMS const &a_lat
    );
    int get(
        std::vector<struct GisRecord *> &mas
      , DMS const &a_long
      , DMS const &a_lat
      , unsigned const p_long
      , unsigned const p_lat
    );
    size_t features_count(void);
    size_t longest_probe(void);
    size_t loc_count(void);
    size_t avg_name(void);
};

#endif /* __GIS_H_ */
