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
        for (int i = 0; i < static_cast<int>(args.size()); i++)
        {
            priv->_out->add(" ");
            priv->_out->add(args[static_cast<unsigned long>(i)]);
        }
        priv->_out->add("\n");
        priv->_out->add("------------------------------------------------------------------------------------------\n");
        priv->_out->add("						World boundaries are set to:\n");
        priv->_out->add("						         ");
        priv->_out->add(std::to_string(dms_to_total(a_long)));
        priv->_out->add("\n");
        priv->_out->add("					");
        priv->_out->add(std::to_string(dms_to_total(a_lat)));
        priv->_out->add("                ");
        priv->_out->add(std::to_string(dms_to_total(b_lat)));
        priv->_out->add("\n");
        priv->_out->add("						         ");
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
        priv->_out->add("\n");
        priv->_out->add("Command ");
        priv->_out->add(std::to_string(priv->_comm_num));
        priv->_out->add(": ");
        for (int i = 0; i < static_cast<int>(args.size()); i++)
        {
            priv->_out->add(" ");
            priv->_out->add(args[static_cast<unsigned long>(i)]);
        }
        priv->_out->add("\n------------------------------------------------------------------------------------------\n");
        priv->_out->add("Successfully imported!\n");
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
    }

    priv->set_state(false);

exit:
    return rc;
}

int CommandProcessor::what_is_at(CommandProcessor *priv, std::vector<std::string> &args)
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

exit:
    return rc;
}

