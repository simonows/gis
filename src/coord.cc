#include <gis/coord.h>


void Coord::remove(struct coord const *arg)
{
    
}



int add_new_elem(struct coord *top, GisRecord const* arg);
{
    int rc = 0;

    if (!top || !arg)
    {
        rc = EINVAL;
        goto exit;
    }

    if (top->next[0] != nullptr)
    {
        // Решить куда идти
        for (int i = 0; i < K && rc == 0; i++)
        {
            rc = add_new_elem(top->next[i], arg);
        }
        goto exit;
    }

    if (top->mas.size() < K)
    {
        top->mas.push_back(arg);
        rc = 1;
    }
    else
    {
        for (int i = 0; i < K; i++)
        {
            top->next[i] = new struct coord;
            for (int j = 0; j < K; j++)
            {
                top->next[i]->next[j] = nullptr;
            }
            top->next[0]->bounds[0] = top->bounds[0];
            top->next[0]->bounds[1] = center(top->bounds[0], top->bounds[1]);

            top->next[3]->bounds[0] = center(top->bounds[0], top->bounds[1]);
            top->next[3]->bounds[1] = top->bounds[1];
        }
        // раскидать 4 элемента
    }

exit:
    return rc;
}


int Coord::add(GisRecord const* arg)
{
    int rc = 0;

    rc = add_new_elem(top, arg);

exit:
    return rc;
}

long Coord::search(DMS const* arg)
{
    
}

void Coord::set_bounds(DMS const a, DMS const b, DMS const c, DMS const d)
{
    long temp = 0;

    bounds_a[0] = make_second(a);
    bounds_a[1] = make_second(b);
    bounds_b[0] = make_second(c);
    bounds_b[1] = make_second(d);

    if (bounds_a[0] > bounds_a[1])
    {
        temp = bounds_a[0];
        bounds_a[1] = bounds_a[0];
        bounds_a[0] = temp;
        temp = bounds_b[0];
        bounds_b[1] = bounds_b[0];
        bounds_b[0] = temp;
    }

    top->bounds_a[0] = bounds_a[0];
    top->bounds_a[1] = bounds_a[1];
    top->bounds_b[0] = bounds_b[0];
    top->bounds_b[1] = bounds_b[1];
}

