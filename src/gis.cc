#include <gis/gis.h>


void Gis::set_bounds(DMS a_long, DMS a_lat, DMS b_long, DMS b_lat)
{
    A[0] = a_long;
    A[1] = a_lat;
    B[0] = b_long;
    B[1] = b_lat;
    start_flag = false;
}


bool Gis::get_flag()
{
    return start_flag;
}

