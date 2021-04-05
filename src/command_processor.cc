#include <cstring>

#include "command_processor.h"
#include "trim.h"


static char const SEPARATOR[] = { "\t\n" };

struct CommandHandler CommandProcessor::_command_pattern[] = {
    "world", CommandProcessor::world
  , "import", CommandProcessor::import
  , "debug", CommandProcessor::debug
  , "quit", CommandProcessor::quit
  , "what_is_at", CommandProcessor::what_is_at
  , "what_is", CommandProcessor::what_is
  , "what_is_in", CommandProcessor::what_is_in
};


CommandProcessor::CommandProcessor(Gis *const gis)
{
    _gis = gis;
    _log = Logger::get_logger();
    _out = new Printer();
    _line = 0;
    _comm_num = 1;
    _run_flag = 1;
}


CommandProcessor::~CommandProcessor()
{
}


bool CommandProcessor::get_state()
{
    return _run_flag;
}


void CommandProcessor::set_state(bool const flag)
{
    _run_flag = flag;
}


int CommandProcessor::command_parse(
    std::string const &comm
  , std::vector<std::string> &args
){
    int check = 0;
    int rc = -1;

    const char *temp = comm.c_str();
    char *word = nullptr;
    char command[MAX_STRING_SIZE] = { 0 };

    strcpy(command, temp);

    args.clear();
    word = strtok(command, SEPARATOR);

    while (word != nullptr)
    {
        args.push_back(std::string(word));
        word = strtok(nullptr, SEPARATOR);
    }

    check = sizeof(_command_pattern) / sizeof(struct CommandHandler);
    for (int i = 0; i < check; i++)
    {
        if (strcmp(args[0].c_str(), _command_pattern[i].command) != 0)
        {
            continue;
        }
        rc = i;
        break;
    }

    return rc;
}

int CommandProcessor::_kernel(std::string const &buffer)
{
    int rc = 0;
    int check = 0;
    std::vector<std::string> args;

    if (!_out)
    {
        rc = EIO;
        goto exit;
    }

    if (buffer.size() == 0)
    {
        goto exit;
    }

    if (buffer[0] == COMMENT_SYMBOL)
    {
        if (_out)
        {
            _out->add(buffer);
        }
        goto exit;
    }

    if ((check = command_parse(buffer, args)) < 0)
    {
        rc = EIO;
        if (_log)
        {
            *_log << WARN << "CommandProcessor::_kernel() ";
            *_log << _log->get_input_file() << ":" << _line << " ";
            *_log << "can't identify command \"" << args[0] << "\"";
            *_log << std::endl;
        }
        goto exit;
    }

    rc = _command_pattern[check].handler(this, args);
    if (rc)
    {
        rc = ENOSYS;
        if (_log)
        {
            *_log << ERROR << "CommandProcessor::_kernel() ";
            *_log << _log->get_input_file() << ":" << _line << " ";
            *_log << "can't execute command \"" << args[0] << "\"";
            *_log << std::endl;
        }
        goto exit;
    }
    _comm_num++;


exit:
    args.clear();
    return rc;
}


char* operator>> (char *const in, CommandProcessor &value)
{
    int rc = 0;

    value._line++;

    rc = value._kernel(trim(std::string(in)));

    if (rc)
    {
        *value._log << ERROR << "CommandProcessor::_kernel() return ";
        *value._log << rc << " code" << std::endl;
    }

    return in;
}

std::ifstream& operator>> (std::ifstream& in, CommandProcessor &value)
{
    std::string temp;
    in >> temp;
    int rc = 0;

    value._line++;

    rc = value._kernel(temp);

    if (rc)
    {
        *value._log << ERROR << "CommandProcessor::_kernel() return ";
        *value._log << rc << " code" << std::endl;
    }

    return in;
}

std::ofstream& operator<< (std::ofstream& out, CommandProcessor const &value)
{
    if (value._out)
    {
        out << *value._out;
    }
    return out;
}
