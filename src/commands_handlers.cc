#include "command_processor.h"


int CommandProcessor::world(CommandProcessor const *priv, std::vector<std::string> &args)
{
    int rc = 0;
    if (priv->_out)
    {
        priv->_out->add("govno");
    }
    return rc;
}

int CommandProcessor::import(CommandProcessor const *priv, std::vector<std::string> &args)
{
    int rc = 0;
    return rc;
}

int CommandProcessor::debug(CommandProcessor const *priv, std::vector<std::string> &args)
{
    int rc = 0;
    return rc;
}

int CommandProcessor::quit(CommandProcessor const *priv, std::vector<std::string> &args)
{
    int rc = 0;
    return rc;
}

int CommandProcessor::what_is_at(CommandProcessor const *priv, std::vector<std::string> &args)
{
    int rc = 0;
    return rc;
}

int CommandProcessor::what_is(CommandProcessor const *priv, std::vector<std::string> &args)
{
    int rc = 0;
    return rc;
}

int CommandProcessor::what_is_in(CommandProcessor const *priv, std::vector<std::string> &args)
{
    int rc = 0;
    return rc;
}

