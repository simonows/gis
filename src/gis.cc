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
    quad.set_bounds(a_long, a_lat, b_long, b_lat);
    start_flag = true;
    is_opened = false;
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
    if (!is_opened)
    {
        std::ofstream ofs(_db); // file create
        ofs.close();
        _data.open(_db, std::ios::out | std::ios::in);
        is_opened = true;
    }

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


static GisRecord *parse_gis(char *const arg)
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
    long pos = _data.tellp();
    long value = 0;
    std::vector<long> mas;

    _data.clear();
    _data << arg << std::endl;
    rec = parse_gis(arg);
    if (!rec)
    {
        goto exit;
    }

    // unique names count
    if (!name_table.get(rec->feature_name, value))
    {
        features_by_name_count++;
        avg_name_length += rec->feature_name.length();
    }

    // unique locations count
    quad.search(mas, &rec->longtitude, &rec->latitude);
    if (mas.size() == 0)
    {
        imported_location_count++;
    }

    hash_table.put(rec->feature_name + rec->state, pos);
    name_table.put(rec->feature_name, pos);
    rec->row = pos;
    quad.add(rec);

exit:
    delete rec;
}

struct GisRecord *Gis::get(std::string name, std::string state)
{
    char buffer[256] = { 0 };
    size_t pool_size = pool.size();
    long value = 0;
    GisRecord *rec = nullptr;

    if (!hash_table.get(name + state, value))
    {
        return nullptr;
    }

    for (size_t i = 0; i < pool_size; i++)
    {
        if (pool[i].row != value)
        {
            continue;
        }

        rec = new GisRecord;
        *rec = pool[i];
        return rec;
    }

    _data.seekg(value);
    _data.getline(buffer, 256);
    _data.seekg(0);
    rec = parse_gis(buffer);
    rec->row = value;

    if (pool_size >= POOL_SIZE)
    {
        pool.erase(pool.begin());
    }

    pool.push_back(*rec);
    return rec;
}

int Gis::get(std::vector<struct GisRecord *> &mas, DMS const &a_long, DMS const &a_lat)
{
    int rc = 0;
    char buffer[256] = { 0 };
    size_t pool_size = pool.size();
    std::vector<long> ret;
    struct GisRecord *temp = nullptr;

    quad.search(ret, &a_long, &a_lat);

    for (size_t i = 0; i < ret.size(); i++)
    {
        _data.seekg(ret[i]);
        _data.getline(buffer, 256);
        _data.seekg(0);
        temp = parse_gis(buffer);
        temp->row = ret[i];
        mas.push_back(temp);

        if (pool_size >= POOL_SIZE)
        {
            pool.erase(pool.begin());
        }
        pool.push_back(*temp);
    }

exit:
    return rc;
}


int Gis::get(
    std::vector<struct GisRecord *> &mas
  , DMS const &a_long
  , DMS const &a_lat
  , unsigned const p_long
  , unsigned const p_lat
){
    int rc = 0;
    char buffer[256] = { 0 };
    size_t pool_size = pool.size();
    std::vector<long> ret;
    struct GisRecord *temp = nullptr;

    quad.search(ret, &a_long, &a_lat, p_long, p_lat);

    for (size_t i = 0; i < ret.size(); i++)
    {
        _data.seekg(ret[i]);
        _data.getline(buffer, 256);
        _data.seekg(0);
        temp = parse_gis(buffer);
        temp->row = ret[i];
        mas.push_back(temp);

        if (pool_size >= POOL_SIZE)
        {
            pool.erase(pool.begin());
        }
        pool.push_back(*temp);
    }

exit:
    return rc;
}


size_t Gis::features_count(void)
{
    return features_by_name_count;
}


size_t Gis::longest_probe(void)
{
    return longest_probe_count;
}


size_t Gis::loc_count(void)
{
    return imported_location_count;
}


size_t Gis::avg_name(void)
{
    return features_by_name_count ? avg_name_length / features_by_name_count : 0;
}


void Gis::get_hash(std::vector<struct GisRecordWithHash> &mas)
{
    struct GisRecord *temp2 = nullptr;
    struct GisRecordWithHash temp;
    char buffer[256] = { 0 };
    std::vector<long> temp_long;
    std::vector<unsigned long> temp_hash;

    hash_table.getAll(temp_long, temp_hash);
    for (size_t i = 0; i < temp_long.size(); i++)
    {
        _data.seekg(temp_long[i]);
        _data.getline(buffer, 256);
        _data.seekg(0);
        temp2 = parse_gis(buffer);
        temp2->row = temp_long[i];

        temp.rec = *temp2;
        temp.hash = temp_hash[i];
        mas.push_back(temp);
        delete temp2;
    }
}
