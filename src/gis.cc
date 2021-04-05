#include <cstdlib>
#include <gis/gis.h>

#define POOL_SIZE 15
#define SEPARATOR2 "|"
#define SEPARATOR3 "/"

void Gis::set_bounds(DMS a_long, DMS a_lat, DMS b_long, DMS b_lat)
{
    A[0] = a_long;
    A[1] = a_lat;
    B[0] = b_long;
    B[1] = b_lat;
    start_flag = true;
}


bool Gis::get_flag()
{
    return start_flag;
}


void Gis::set_db_file(char const *const arg)
{
    _db = arg;
}


int Gis::open()
{
    std::ofstream ofs(_db); // file create
    ofs.close();

    _data.open(_db, std::ios::out | std::ios::in);
    return !_data.is_open();
}


static int parse_dms(std::string arg, struct DMS &result)
{
    int rc = 0;

    if (arg.size() == 7)
    {
        result.sign_long = 0;
        result.sign_lat = 0;
        if (arg[6] != 'W' && arg[6] != 'N')
        {
            rc = 3;
            goto exit;
        }
        sscanf(arg.c_str(), "%2u%2u%2uW"
          , &result.degrees
          , &result.minutes
          , &result.seconds
        );
    }
    else
    {
        result.sign_long = 1;
        result.sign_lat = 1;
        if (arg[7] != 'W' && arg[7] != 'N')
        {
            rc = 3;
            goto exit;
        }
        if (arg[6] == 'W')
        {
            result.sign_lat = 1;
        }
        if (arg[6] == 'N')
        {
            result.sign_long = 1;
        }
        sscanf(arg.c_str(), "0%2u%2u%2uW"
          , &result.degrees
          , &result.minutes
          , &result.seconds
        );
    }

exit:
    return rc;
}


/* behaves like strtok() except that it returns empty tokens also
 */
static char* strtoke(char *str, const char *delim)
{
    static char *start = NULL; /* stores string str for consecutive calls */
    char *token = NULL; /* found token */
    /* assign new start in case */
    if (str) start = str;
    /* check whether text to parse left */
    if (!start) return NULL;
    /* remember current start as found token */
    token = start;
    /* find next occurrence of delim */
    start = strpbrk(start, delim);
    /* replace delim with terminator and move start to follower */
    if (start) *start++ = '\0';
    /* done */
    return token;
}


static GisRecord *parse_gis(Logger *log, char *const arg)
{
    GisRecord *rec = new GisRecord;
    char *word = nullptr;

    word = strtoke(arg, SEPARATOR2);
    if (word == nullptr)
    {
        goto exit;
    }
    rec->feature_id = atoi(word);
    word = strtoke(nullptr, SEPARATOR2);
    if (word == nullptr)
    {
        goto exit;
    }
    rec->feature_name = word;
    word = strtoke(nullptr, SEPARATOR2);
    if (word == nullptr)
    {
        goto exit;
    }
    rec->feature_cat = word;
    word = strtoke(nullptr, SEPARATOR2);
    if (word == nullptr)
    {
        goto exit;
    }
    rec->state = word;
    word = strtoke(nullptr, SEPARATOR2);
    word = strtoke(nullptr, SEPARATOR2);
    if (word == nullptr)
    {
        goto exit;
    }
    rec->country = word;
    word = strtoke(nullptr, SEPARATOR2);
    word = strtoke(nullptr, SEPARATOR2);
    if (word == nullptr)
    {
        goto exit;
    }
    parse_dms(std::string(word), rec->longtitude);
    word = strtoke(nullptr, SEPARATOR2);
    if (word == nullptr)
    {
        goto exit;
    }
    parse_dms(std::string(word), rec->latitude);

    word = strtoke(nullptr, SEPARATOR2);
    word = strtoke(nullptr, SEPARATOR2);
    word = strtoke(nullptr, SEPARATOR2);
    word = strtoke(nullptr, SEPARATOR2);
    word = strtoke(nullptr, SEPARATOR2);
    word = strtoke(nullptr, SEPARATOR2);
    word = strtoke(nullptr, SEPARATOR2);

    word = strtoke(nullptr, SEPARATOR2);
    if (word == nullptr)
    {
        goto exit;
    }
    rec->elev = atoi(word);
    word = strtoke(nullptr, SEPARATOR2);
    if (word == nullptr)
    {
        goto exit;
    }
    rec->usgs = word;
    word = strtoke(nullptr, SEPARATOR3);
    if (word == nullptr)
    {
        goto exit;
    }
    rec->month = atoi(word);
    word = strtoke(nullptr, SEPARATOR3);
    if (word == nullptr)
    {
        goto exit;
    }
    rec->day = atoi(word);
    word = strtoke(nullptr, SEPARATOR3);
    if (word == nullptr)
    {
        goto exit;
    }
    rec->year = atoi(word);

    return rec;
exit:
    delete rec;
    rec = nullptr;
    return rec;
}

void Gis::add(char *const arg)
{
    GisRecord *rec = nullptr;
    unsigned long pos = static_cast<unsigned long>(_data.tellp());

    _data << arg << std::endl;
    rec = parse_gis(log, arg);
    if (!rec)
    {
        goto exit;
    }

    hash_table[rec->feature_name + rec->state] = pos;

exit:
    delete rec;
}

struct GisRecord *Gis::get(std::string name, std::string state)
{
    char buffer[256] = { 0 };
    size_t pool_size = pool.size();

    std::map <std::string, unsigned long> :: iterator it = hash_table.find(name + state);
    GisRecord *rec = nullptr;

    if (it == hash_table.end())
    {
        return nullptr;
    }

    for (size_t i = 0; i < pool_size; i++)
    {
        if (pool[i].row != static_cast<long>(it->second))
        {
            continue;
        }

        rec = new GisRecord;
        *rec = pool[i];
        return rec;
    }

    _data.seekg(static_cast<long>(it->second));
    _data.getline(buffer, 256);
    _data.seekg(0);
    rec = parse_gis(log, buffer);
    rec->row = static_cast<long>(it->second);

    if (pool_size >= POOL_SIZE)
    {
        pool.erase(pool.begin());
    }

    pool.push_back(*rec);
    return rec;
}

