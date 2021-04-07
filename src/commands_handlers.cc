#include <cstdio>
#include <gis/gis.h>

#include "command_processor.h"

static int parse_dms(std::string &arg, struct DMS &result)
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

static inline int dms_to_total(struct DMS const arg)
{
    return (arg.sign_lat || arg.sign_long ? -1 : 1)
         * static_cast<int>((arg.minutes * 60 + arg.degrees * 3600 + arg.seconds));
}

int CommandProcessor::world(CommandProcessor *priv, std::vector<std::string> &args)
{
    int rc = 0;
    struct DMS a_long, a_lat;
    struct DMS b_long, b_lat;

    if (!priv)
    {
        rc = EINVAL;
        goto exit;
    }
    if (!priv->_gis)
    {
        rc = EINVAL;
        goto exit;
    }

    rc = parse_dms(args[1], a_lat);
    if (rc)
    {
        goto exit;
    }
    rc = parse_dms(args[2], b_lat);
    if (rc)
    {
        goto exit;
    }
    rc = parse_dms(args[3], a_long);
    if (rc)
    {
        goto exit;
    }
    rc = parse_dms(args[4], b_long);
    if (rc)
    {
        goto exit;
    }

    priv->_gis->set_bounds(a_long, a_lat, b_long, b_lat);
    if (priv->_out)
    {
        for (int i = 0; i < static_cast<int>(args.size() - 1); i++)
        {
            priv->_out->add(args[static_cast<unsigned long>(i)]);
            priv->_out->add("\t");
        }
        priv->_out->add(args[args.size() - 1]);

        priv->_out->add("\n\n------------------------------------------------------------------------------------------\n");
        priv->_out->add("Latitude/longitude values in index entries are shown as signed integers, in total seconds.");
        priv->_out->add("\n");
        priv->_out->add("------------------------------------------------------------------------------------------\n");
        priv->_out->add("                        World boundaries are set to:\n");
        priv->_out->add("                                   ");
        priv->_out->add(std::to_string(dms_to_total(a_long)));
        priv->_out->add("\n");
        priv->_out->add("                        ");
        priv->_out->add(std::to_string(dms_to_total(a_lat)));
        priv->_out->add("              ");
        priv->_out->add(std::to_string(dms_to_total(b_lat)));
        priv->_out->add("\n");
        priv->_out->add("                                   ");
        priv->_out->add(std::to_string(dms_to_total(b_long)));
        priv->_out->add("\n");
        priv->_out->add("------------------------------------------------------------------------------------------\n");
    }

exit:
    return rc;
}

int CommandProcessor::import(CommandProcessor *priv, std::vector<std::string> &args)
{
    int rc = 0;
    std::ifstream thread;
    char buffer[1024] = { 0 };

    if (!priv)
    {
        rc = EINVAL;
        goto exit;
    }
    if (!priv->_gis)
    {
        rc = EINVAL;
        goto exit;
    }

    if (!priv->_gis->get_flag())
    {
        *priv->_log << WARN << priv->_log->get_input_file() << ":" << priv->_line << " ";
        *priv->_log << "world is not set:" << std::endl;
        goto exit;
    }

    rc = priv->_gis->open();
    if (rc)
    {
        goto exit;
    }

    thread.open(args[1], std::ios::in);
    rc = !thread.is_open();
    if (rc)
    {
        *priv->_log << WARN << priv->_log->get_input_file() << ":" << priv->_line << " ";
        *priv->_log << "file not found:  \"" << args[1] << "\"";
        *priv->_log << std::endl;
        goto exit;
    }

    thread.getline(buffer, 1024);

    while (!thread.eof())
    {
        thread.getline(buffer, 1024);
        priv->_gis->add(buffer);
    }
    thread.close();

    if (priv->_out)
    {
        priv->_out->add("Command ");
        priv->_out->add(std::to_string(priv->_comm_num));
        priv->_out->add(": ");
        for (int i = 0; i < static_cast<int>(args.size()); i++)
        {
            priv->_out->add("\t");
            priv->_out->add(args[static_cast<unsigned long>(i)]);
        }
        priv->_out->add("\n\n");
        priv->_out->add("Imported Features by name: ");
        priv->_out->add(std::to_string(priv->_gis->features_count()));
        priv->_out->add("\n");
        priv->_out->add("Longest probe sequence:    ");
        priv->_out->add(std::to_string(priv->_gis->longest_probe()));
        priv->_out->add("\n");
        priv->_out->add("Imported Locations:        ");
        priv->_out->add(std::to_string(priv->_gis->loc_count()));
        priv->_out->add("\n");
        priv->_out->add("Average name length:       ");
        priv->_out->add(std::to_string(priv->_gis->avg_name()));
        priv->_out->add("\n------------------------------------------------------------------------------------------\n");
    }

exit:
    return rc;
}

int CommandProcessor::debug(CommandProcessor *priv, std::vector<std::string> &args)
{
    int rc = 0;

    if (!priv)
    {
        rc = EINVAL;
        goto exit;
    }
    if (!priv->_gis)
    {
        rc = EINVAL;
        goto exit;
    }
    if (!priv->_gis->get_flag())
    {
        *priv->_log << WARN << priv->_log->get_input_file() << ":" << priv->_line << " ";
        *priv->_log << "world is not set:" << std::endl;
        goto exit;
    }

    if (!priv->_out)
    {
        goto exit;
    }

    priv->_out->add("Command ");
    priv->_out->add(std::to_string(priv->_comm_num));
    priv->_out->add(": ");
    for (int i = 0; i < static_cast<int>(args.size()); i++)
    {
        priv->_out->add("\t");
        priv->_out->add(args[static_cast<unsigned long>(i)]);
    }
    priv->_out->add("\n\n");

    if (args[1] == "world")
    {
        priv->_out->add("This command optional to implement");
        priv->_out->add("\n------------------------------------------------------------------------------------------\n");
    }
    else if (args[1] == "quad")
    {
        priv->_out->add("This command optional to implement");
        priv->_out->add("\n------------------------------------------------------------------------------------------\n");
    }
    else if (args[1] == "hash")
    {
        std::vector<struct GisRecordWithHash> hash;
        priv->_gis->get_hash(hash);

        priv->_out->add("Format of display is \n");
        priv->_out->add("Slot number: data record\n");
        priv->_out->add("Current table size is 1024\n");
        priv->_out->add("Number of elements in table is ");
        priv->_out->add(std::to_string(hash.size()));
        priv->_out->add("\n\n");

        for (size_t i = 0; i < hash.size(); i++)
        {
            priv->_out->add("  ");
            priv->_out->add(std::to_string(hash[i].rec.row));
            priv->_out->add(":  [");
            priv->_out->add(hash[i].rec.feature_name);
            priv->_out->add(": ");
            priv->_out->add(hash[i].rec.state);
            priv->_out->add(" [");
            priv->_out->add(std::to_string(hash[i].hash));
            priv->_out->add("]");
            priv->_out->add("]\n");
        }

        priv->_out->add("------------------------------------------------------------------------------------------\n");
    }
    else if (args[1] == "pool")
    {
        auto ptr = priv->_gis->get_pool();
        priv->_out->add("MRU\n");
        for (size_t i = 0; i < ptr->size(); i++)
        {
            priv->_out->add("  ");
            priv->_out->add(std::to_string((*ptr)[i].row));
            priv->_out->add(":  ");
            priv->_out->add((*ptr)[i].country);
            priv->_out->add("  (");
            priv->_out->add(std::to_string((*ptr)[i].longtitude.degrees) + "d ");
            priv->_out->add(std::to_string((*ptr)[i].longtitude.minutes) + "m ");
            priv->_out->add(std::to_string((*ptr)[i].longtitude.seconds) + "s North, ");
            priv->_out->add(std::to_string((*ptr)[i].latitude.degrees) + "d ");
            priv->_out->add(std::to_string((*ptr)[i].latitude.minutes) + "m ");
            priv->_out->add(std::to_string((*ptr)[i].latitude.seconds) + "s West)\n");

        }
        priv->_out->add("LRU\n");
        priv->_out->add("------------------------------------------------------------------------------------------\n");
    }

exit:
    return rc;
}

int CommandProcessor::quit(CommandProcessor *priv, std::vector<std::string> &args)
{
    int rc = 0;
    auto end = std::chrono::system_clock::now();
    std::time_t end_time = 0;
    std::string now_time = "";

    if (!priv)
    {
        rc = EINVAL;
        goto exit;
    }
    if (!priv->_gis)
    {
        rc = EINVAL;
        goto exit;
    }

    if (priv->_out)
    {
        end_time = std::chrono::system_clock::to_time_t(end);
        now_time = std::ctime(&end_time);
        priv->_out->add("Command ");
        priv->_out->add(std::to_string(priv->_comm_num));
        priv->_out->add(": ");
        priv->_out->add(args[0]);
        priv->_out->add("\n\n");
        priv->_out->add("Terminating execution of commands.\n");
        priv->_out->add("------------------------------------------------------------------------------------------\n");
        priv->_out->add("End time: ");
        priv->_out->add(now_time.substr(0, now_time.size() - 1));
        *priv->_log << INFO << "GIS system terminated" << std::endl;
    }

    priv->set_state(false);

exit:
    return rc;
}

int CommandProcessor::what_is_at(CommandProcessor *priv, std::vector<std::string> &args)
{
    int rc = 0;
    DMS long_d, lat_d;
    std::vector<struct GisRecord *> mas;

    if (!priv)
    {
        rc = EINVAL;
        goto exit;
    }
    if (!priv->_gis)
    {
        rc = EINVAL;
        goto exit;
    }
    if (!priv->_gis->get_flag())
    {
        *priv->_log << WARN << priv->_log->get_input_file() << ":" << priv->_line << " ";
        *priv->_log << "world is not set:" << std::endl;
        goto exit;
    }

    rc = parse_dms(args[1], long_d);
    if (rc)
    {
        goto exit;
    }
    rc = parse_dms(args[2], lat_d);
    if (rc)
    {
        goto exit;
    }

    if (priv->_out)
    {
        priv->_out->add("Command ");
        priv->_out->add(std::to_string(priv->_comm_num));
        priv->_out->add(": ");
        for (size_t i = 0; i < args.size(); i++)
        {
            priv->_out->add(" ");
            priv->_out->add(args[static_cast<unsigned long>(i)]);
        }
        priv->_out->add("\n\n");
    }

    priv->_gis->get(mas, long_d, lat_d);
    if (mas.size() && priv->_out)
    {
        priv->_out->add("  The following ");
        priv->_out->add(std::to_string(mas.size()));
        priv->_out->add(" feature(s) were found at (");
        priv->_out->add(std::to_string(long_d.degrees));
        priv->_out->add("d ");
        priv->_out->add(std::to_string(long_d.minutes));
        priv->_out->add("m ");
        priv->_out->add(std::to_string(long_d.seconds));
        priv->_out->add("s North, ");
        priv->_out->add(std::to_string(lat_d.degrees));
        priv->_out->add("d ");
        priv->_out->add(std::to_string(lat_d.minutes));
        priv->_out->add("m ");
        priv->_out->add(std::to_string(lat_d.seconds));
        priv->_out->add("s West)\n");

        for (size_t i = 0; i < mas.size(); i++)
        {
            priv->_out->add("    ");
            priv->_out->add(std::to_string(mas[i]->row));
            priv->_out->add(":  \"");
            priv->_out->add(mas[i]->feature_name);
            priv->_out->add("\"  \"");
            priv->_out->add(mas[i]->country);
            priv->_out->add("\"  \"");
            priv->_out->add(mas[i]->state);
            priv->_out->add("\n");
            delete mas[i];
        }

        priv->_out->add("------------------------------------------------------------------------------------------\n");
    }
    else
    {
        priv->_out->add("  Nothing was found at (");
        priv->_out->add(std::to_string(long_d.degrees));
        priv->_out->add("d ");
        priv->_out->add(std::to_string(long_d.minutes));
        priv->_out->add("m ");
        priv->_out->add(std::to_string(long_d.seconds));
        priv->_out->add("s North, ");
        priv->_out->add(std::to_string(lat_d.degrees));
        priv->_out->add("d ");
        priv->_out->add(std::to_string(lat_d.minutes));
        priv->_out->add("m ");
        priv->_out->add(std::to_string(lat_d.seconds));
        priv->_out->add("s West)\n");
        priv->_out->add("------------------------------------------------------------------------------------------\n");
    }

exit:
    return rc;
}

int CommandProcessor::what_is(CommandProcessor *priv, std::vector<std::string> &args)
{
    int rc = 0;
    long row_num = 0;
    GisRecord *rec = nullptr;

    if (!priv)
    {
        rc = EINVAL;
        goto exit;
    }
    if (!priv->_gis)
    {
        rc = EINVAL;
        goto exit;
    }
    if (!priv->_gis->get_flag())
    {
        *priv->_log << WARN << priv->_log->get_input_file() << ":" << priv->_line << " ";
        *priv->_log << "world is not set:" << std::endl;
        goto exit;
    }

    if (priv->_out)
    {
        priv->_out->add("Command ");
        priv->_out->add(std::to_string(priv->_comm_num));
        priv->_out->add(": ");
        for (size_t i = 0; i < args.size(); i++)
        {
            priv->_out->add(" ");
            priv->_out->add(args[static_cast<unsigned long>(i)]);
        }
        priv->_out->add("\n\n");
    }

    rec = priv->_gis->get(args[1], args[2]);
    if (priv->_log && rec)
    {
        if (priv->_out)
        {
            priv->_out->add("  ");
            priv->_out->add(std::to_string(rec->row));
            priv->_out->add(":  ");
            priv->_out->add(rec->country);
            priv->_out->add("  (");
            priv->_out->add(std::to_string(rec->longtitude.degrees) + "d ");
            priv->_out->add(std::to_string(rec->longtitude.minutes) + "m ");
            priv->_out->add(std::to_string(rec->longtitude.seconds) + "s North, ");
            priv->_out->add(std::to_string(rec->latitude.degrees) + "d ");
            priv->_out->add(std::to_string(rec->latitude.minutes) + "m ");
            priv->_out->add(std::to_string(rec->latitude.seconds) + "s West)\n");
            priv->_out->add("------------------------------------------------------------------------------------------\n");
        }
        delete rec;
    }
    else
    {
        if (priv->_out)
        {
            priv->_out->add("  No records match \"");
            priv->_out->add(args[1]);
            priv->_out->add("\" and \"");
            priv->_out->add(args[2]);
            priv->_out->add("\"\n");
            priv->_out->add("------------------------------------------------------------------------------------------\n");
        }
    }


exit:
    return rc;
}

int CommandProcessor::what_is_in(CommandProcessor *priv, std::vector<std::string> &args)
{
    int rc = 0;
    DMS long_d, lat_d;
    unsigned long_p, lat_p;
    size_t cou = 0;
    std::vector<struct GisRecord *> mas;

    if (!priv)
    {
        rc = EINVAL;
        goto exit;
    }
    if (!priv->_gis)
    {
        rc = EINVAL;
        goto exit;
    }
    if (!priv->_gis->get_flag())
    {
        *priv->_log << WARN << priv->_log->get_input_file() << ":" << priv->_line << " ";
        *priv->_log << "world is not set:" << std::endl;
        goto exit;
    }

    if (args[1] == "-filter")
    {
        cou = 2;
    }
    if (args[1] == "-long")
    {
        cou = 1;
    }
    rc = parse_dms(args[1 + cou], long_d);
    if (rc)
    {
        goto exit;
    }
    rc = parse_dms(args[2 + cou], lat_d);
    if (rc)
    {
        goto exit;
    }
    long_p = static_cast<unsigned>(atoi(args[3 + cou].c_str()));
    lat_p = static_cast<unsigned>(atoi(args[4 + cou].c_str()));

    if (priv->_out)
    {
        priv->_out->add("Command ");
        priv->_out->add(std::to_string(priv->_comm_num));
        priv->_out->add(": ");
        for (size_t i = 0; i < args.size(); i++)
        {
            priv->_out->add(" ");
            priv->_out->add(args[static_cast<unsigned long>(i)]);
        }
        priv->_out->add("\n\n");
    }

    priv->_gis->get(mas, long_d, lat_d, long_p, lat_p);
    if (mas.size() && priv->_out)
    {
        priv->_out->add("  The following ");
        priv->_out->add(std::to_string(mas.size()));
        priv->_out->add(" feature(s) were found at (");
        priv->_out->add(std::to_string(long_d.degrees));
        priv->_out->add("d ");
        priv->_out->add(std::to_string(long_d.minutes));
        priv->_out->add("m ");
        priv->_out->add(std::to_string(long_d.seconds));
        priv->_out->add("s North +/- ");
        priv->_out->add(std::to_string(long_p));
        priv->_out->add(", ");
        priv->_out->add(std::to_string(lat_d.degrees));
        priv->_out->add("d ");
        priv->_out->add(std::to_string(lat_d.minutes));
        priv->_out->add("m ");
        priv->_out->add(std::to_string(lat_d.seconds));
        priv->_out->add("s West +/- ");
        priv->_out->add(std::to_string(lat_p));
        priv->_out->add(")\n");

        for (size_t i = 0; i < mas.size(); i++)
        {
            priv->_out->add("    ");
            priv->_out->add(std::to_string(mas[i]->row));
            priv->_out->add(":  \"");
            priv->_out->add(mas[i]->feature_name);
            priv->_out->add("\"  \"");
            priv->_out->add(mas[i]->state);
            priv->_out->add("\"  \"(");
            priv->_out->add(std::to_string(mas[i]->longtitude.degrees));
            priv->_out->add("d ");
            priv->_out->add(std::to_string(mas[i]->longtitude.minutes));
            priv->_out->add("m ");
            priv->_out->add(std::to_string(mas[i]->longtitude.seconds));
            priv->_out->add("s North, ");
            priv->_out->add(std::to_string(mas[i]->latitude.degrees));
            priv->_out->add("d ");
            priv->_out->add(std::to_string(mas[i]->latitude.minutes));
            priv->_out->add("m ");
            priv->_out->add(std::to_string(mas[i]->latitude.seconds));
            priv->_out->add("s West");
            priv->_out->add(")\"\n");
            delete mas[i];
        }

        priv->_out->add("------------------------------------------------------------------------------------------\n");
    }
    else
    {
        priv->_out->add("  Nothing was found at (");
        priv->_out->add(std::to_string(long_d.degrees));
        priv->_out->add("d ");
        priv->_out->add(std::to_string(long_d.minutes));
        priv->_out->add("m ");
        priv->_out->add(std::to_string(long_d.seconds));
        priv->_out->add("s North +/- ");
        priv->_out->add(std::to_string(long_p));
        priv->_out->add(", ");
        priv->_out->add(std::to_string(lat_d.degrees));
        priv->_out->add("d ");
        priv->_out->add(std::to_string(lat_d.minutes));
        priv->_out->add("m ");
        priv->_out->add(std::to_string(lat_d.seconds));
        priv->_out->add("s West +/- ");
        priv->_out->add(std::to_string(lat_p));
        priv->_out->add(")\n");
        priv->_out->add("------------------------------------------------------------------------------------------\n");
    }

exit:
    return rc;
}

