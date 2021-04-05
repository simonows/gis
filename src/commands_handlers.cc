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

exit:
    return rc;
}

int CommandProcessor::what_is(CommandProcessor *priv, std::vector<std::string> &args)
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

exit:
    return rc;
}

