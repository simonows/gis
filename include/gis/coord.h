#ifndef __COORD_H__
#define __COORD_H__

#include <fstream>
#include <vector>
#include <cstring>

#include <gis/logger.h>
#include <gis/gis.h>

#define QUAD 4
#define k 4

typedef struct coord
{
    struct coord *next[QUAD];
    std::vector<GisRecord*> mas;
    long bounds_a[2];
    long bounds_b[2];
} coord;


class Coord
{
    Logger *log;
    struct coord *top;
    DMS bounds_a[2];
    DMS bounds_b[2];

protected:
    void remove(struct coord const *elem);
    static int add_new_elem(struct coord **top, GisRecord const* arg);

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
        remove(top);
    }

    int add(GisRecord const* arg);
    long search(DMS const* arg);
    void set_bounds(DMS const a, DMS const b);
};

#endif /* __COORD_H_ */

