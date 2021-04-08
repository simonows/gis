#include <gis/coord.h>
#include <gis/gis.h>


long make_second(struct DMS const &arg)
{
    return (arg.sign_lat || arg.sign_long ? -1 : 1)
         * static_cast<long>(arg.degrees * 3600 + arg.minutes * 60 + arg.seconds);
}


static void _remove(struct coord const *arg)
{
    if (arg->next[0] == nullptr)
    {
        return;
    }

    for (int i = 0; i < QUAD; i++)
    {
        _remove(arg->next[i]);
        delete arg->next[i];
    }
}


void Coord::remove(void)
{
    _remove(top);
    delete top;
}


int Coord::add_new_elem(struct coord *top, struct coord_rec const* arg)
{
    int rc = 0;
    int check = 0;

    if (!top || !arg)
    {
        rc = EINVAL;
        goto exit;
    }

    if (top->next[0] != nullptr)
    {
        for (int i = 0; i < QUAD; i++)
        {
            if (arg->point[0] > top->next[i]->bounds_a[0]
             && arg->point[1] > top->next[i]->bounds_a[1]
             && arg->point[0] <= top->next[i]->bounds_b[0]
             && arg->point[1] <= top->next[i]->bounds_b[1])
            {
                check = i;
            }
        }

        rc = add_new_elem(top->next[check], arg);

        goto exit;
    }

    if (top->mas.size() < KK)
    {
        top->mas.push_back(*arg);
        rc = 1;
        goto exit;
    }

    for (int i = 0; i < KK; i++)
    {
        top->next[i] = new struct coord;
        for (int j = 0; j < KK; j++)
        {
            top->next[i]->next[j] = nullptr;
        }
    }

    top->next[0]->bounds_a[0] = top->bounds_a[0];
    top->next[0]->bounds_a[1] = top->bounds_a[1];
    top->next[0]->bounds_b[0] = (top->bounds_a[0] + top->bounds_b[0]) / 2;
    top->next[0]->bounds_b[1] = (top->bounds_a[1] + top->bounds_b[1]) / 2;

    top->next[1]->bounds_a[0] = top->bounds_a[0];
    top->next[1]->bounds_a[1] = (top->bounds_a[1] + top->bounds_b[1]) / 2;
    top->next[1]->bounds_b[0] = (top->bounds_a[0] + top->bounds_b[0]) / 2;
    top->next[1]->bounds_b[1] = top->bounds_b[1];

    top->next[2]->bounds_a[0] = (top->bounds_a[0] + top->bounds_b[0]) / 2;
    top->next[2]->bounds_a[1] = (top->bounds_a[1] + top->bounds_b[1]) / 2;
    top->next[2]->bounds_b[0] = top->bounds_b[0];
    top->next[2]->bounds_b[1] = top->bounds_b[1];

    top->next[3]->bounds_a[0] = (top->bounds_a[0] + top->bounds_b[0]) / 2;
    top->next[3]->bounds_a[1] = top->bounds_a[1];
    top->next[3]->bounds_b[0] = top->bounds_b[0];
    top->next[3]->bounds_b[1] = (top->bounds_a[1] + top->bounds_b[1]) / 2;

    top->mas.push_back(*arg);
    for (size_t i = 0; i < KK + 1; i++)
    {
        for (size_t j = 0; j < QUAD; j++)
        {
            if (top->mas[i].point[0] > top->next[j]->bounds_a[0]
             && top->mas[i].point[1] > top->next[j]->bounds_a[1]
             && top->mas[i].point[0] <= top->next[j]->bounds_b[0]
             && top->mas[i].point[1] <= top->next[j]->bounds_b[1])
            {
                top->next[j]->mas.push_back(top->mas[i]);
            }
        }
    }
    top->mas.clear();

exit:
    return rc;
}


int Coord::add(struct GisRecord const* arg)
{
    int rc = 0;
    struct coord_rec temp;

    temp.point[0] = make_second(arg->longtitude);
    temp.point[1] = make_second(arg->latitude);
    temp.seek = arg->row;

    rc = add_new_elem(top, &temp);

exit:
    return rc;
}


static void _search_any(
    struct coord const *top
  , std::vector<long> &result
  , long const long_arg
  , long const lat_arg
  , unsigned long_p
  , unsigned lat_p
){
    /*if (!(top->bounds_a[0] < long_arg - long_p && top->bounds_b[0] >= long_arg - long_p
     && top->bounds_a[1] < lat_arg - lat_p && top->bounds_b[1] >= lat_arg - lat_p))
    {
        goto run;
    }
    else if (!(top->bounds_a[0] < long_arg + long_p && top->bounds_b[0] >= long_arg + long_p
     && top->bounds_a[1] < lat_arg + lat_p && top->bounds_b[1] >= lat_arg + lat_p))
    {
        goto run;
    }
    else if (!(top->bounds_a[0] < long_arg - long_p && top->bounds_b[0] >= long_arg - long_p
     && top->bounds_a[1] < lat_arg + lat_p && top->bounds_b[1] >= lat_arg + lat_p))
    {
        goto run;
    }
    else if (!(top->bounds_a[0] < long_arg + long_p && top->bounds_b[0] >= long_arg + long_p
     && top->bounds_a[1] < lat_arg - lat_p && top->bounds_b[1] >= lat_arg - lat_p))
    {
        goto run;
    }
    else
    {
        return;
    }*/

run:
    if (top->next[0] == nullptr)
    {
        for (size_t i = 0; i < top->mas.size(); i++)
        {
            if (top->mas[i].point[0] > long_arg - long_p
             && top->mas[i].point[0] <= long_arg + long_p
             && top->mas[i].point[1] > lat_arg - lat_p
             && top->mas[i].point[1] <= lat_arg + lat_p
            ){
                result.push_back(top->mas[i].seek);
            }
        }
        return;
    }

    for (int i = 0; i < QUAD; i++)
    {
        _search_any(top->next[i], result, long_arg, lat_arg, long_p, lat_p);
    }
}


long Coord::search(
    std::vector<long> &result
  , DMS const* long_arg
  , DMS const* lat_arg
  , unsigned long_p
  , unsigned lat_p
){
    _search_any(
        top
      , result
      , make_second(*long_arg)
      , make_second(*lat_arg)
      , long_p
      , lat_p
    );

    return 0;
}


static long _search(
    std::vector<long> &mas
  , struct coord const *top
  , long const long_arg
  , long const lat_arg
){
    long ret = -1;

    if (top->bounds_a[0] >= long_arg || top->bounds_b[0] < long_arg)
    {
        return ret;
    }
    if (top->bounds_a[1] >= lat_arg || top->bounds_b[1] < lat_arg)
    {
        return ret;
    }

    if (top->next[0] == nullptr)
    {
        for (size_t i = 0; i < top->mas.size(); i++)
        {
            if (top->mas[i].point[0] == long_arg && top->mas[i].point[1] == lat_arg)
            {
                mas.push_back(top->mas[i].seek);
            }
        }
        return ret;
    }

    for (int i = 0; i < QUAD; i++)
    {
        ret = _search(mas, top->next[i], long_arg, lat_arg);
    }
    return ret;
}


long Coord::search(std::vector<long> &mas, DMS const* long_arg, DMS const* lat_arg)
{
    return _search(mas, top, make_second(*long_arg), make_second(*lat_arg));
}


void Coord::set_bounds(struct DMS const a, DMS const b, DMS const c, DMS const d)
{
    long temp = 0;

    bounds_a[0] = make_second(a);
    bounds_a[1] = make_second(b);
    bounds_b[0] = make_second(c);
    bounds_b[1] = make_second(d);

    if (bounds_a[0] > bounds_b[0] && bounds_a[1] > bounds_b[1])
    {
        temp = bounds_b[0];
        bounds_b[0] = bounds_a[0];
        bounds_a[0] = temp;
        temp = bounds_b[1];
        bounds_b[1] = bounds_a[1];
        bounds_a[1] = temp;
    }

    top->bounds_a[0] = bounds_a[0];
    top->bounds_a[1] = bounds_a[1];
    top->bounds_b[0] = bounds_b[0];
    top->bounds_b[1] = bounds_b[1];
}


static void set_level_str(std::string &buffer, unsigned level)
{
    for (unsigned i = 0; i < level; i++)
    {
        buffer += "   ";
    }
}

static void _print(std::string &buffer, unsigned level, struct coord const *arg)
{
    if (arg->next[0] == nullptr)
    {
        set_level_str(buffer, level);
        if (arg->mas.size() == 0)
        {
            buffer += "*\n";
            return;
        }
        else
        {
            buffer += "[";
            for (size_t i = 0; i < arg->mas.size(); i++)
            {
                buffer += "(" + std::to_string(arg->mas[i].point[0]);
                buffer += "," + std::to_string(arg->mas[i].point[1]);
                buffer += "), ";
                buffer += std::to_string(arg->mas[i].seek);
            }
            buffer += "]\n";
        }
        return;
    }

    for (int i = 0; i < QUAD; i++)
    {
        _print(buffer, level + 1, arg->next[i]);
        if (i == QUAD / 2 - 1)
        {
            set_level_str(buffer, level);
            buffer += "@\n";
        }
    }
}

std::string Coord::print(void)
{
    std::string buffer;

    _print(buffer, 0, top);

    return buffer;
}



