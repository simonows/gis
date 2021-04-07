#ifndef __COORD_H__
#define __COORD_H__

#include <fstream>
#include <vector>
#include <cstring>

#include <gis/logger.h>

#define QUAD 4
#define KK 4


typedef struct DMS
{
    char sign_lat;
    char sign_long;
    unsigned int degrees;
    unsigned int minutes;
    unsigned int seconds;
} DMS;


typedef struct coord_rec
{
    long point[2];
    long seek;
} coord_rec;


typedef struct coord
{
    struct coord *next[QUAD];
    std::vector<struct coord_rec> mas;
    long bounds_a[2];
    long bounds_b[2];
} coord;


long make_second(struct DMS const &arg);


class Coord
{
    Logger *log;
    struct coord *top;
    long bounds_a[2];
    long bounds_b[2];

protected:
    void remove(void);
    static int add_new_elem(struct coord *top, struct coord_rec const* arg);

public:
    Coord()
    {
        log = Logger::get_logger();
        top = new struct coord;
        top->next[0] = nullptr;
        top->next[1] = nullptr;
        top->next[2] = nullptr;
        top->next[3] = nullptr;
    }
    ~Coord()
    {
        remove();
    }

    int add(struct GisRecord const* arg);
    long search(
        std::vector<long> &result
      , struct DMS const* long_arg
      , struct DMS const* lat_arg
      , unsigned long_p
      , unsigned lat_p
    );
    long search(
        std::vector<long> &mas
      , struct DMS const* long_arg
      , struct DMS const* lat_arg
    );
    void set_bounds(struct DMS const a, DMS const b, DMS const c, DMS const d);
};

#endif /* __COORD_H_ */

