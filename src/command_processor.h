#ifndef __COMMAND_PROCESSOR_H__
#define __COMMAND_PROCESSOR_H__

#include <string>
#include <fstream>
#include <vector>

#include <gis/logger.h>
#include <gis/gis.h>

#include "printer.h"

#define COMMENT_SYMBOL ';'
#define MAX_STRING_SIZE 256


typedef enum CommandType
{
    WORLD = 0,
    IMPORT,
    DEBUGCOMM,
    QUIT,
    WHAT_IS_AT,
    WHAT_IS,
    WHAT_IS_IN,
} CommandType;

class CommandProcessor;

typedef struct CommandHandler
{
    char const *command;
    int (*handler)(CommandProcessor const *priv, std::vector<std::string> &args);
} CommandHandler;


class CommandProcessor
{
    Gis const* _gis;
    char const* _script_name;
    Printer *_out;
    Logger *_log;
    int _line;
    static struct CommandHandler _command_pattern[];

protected:
    int _kernel(std::string const &buffer);
    int command_parse(std::string const &comm, std::vector<std::string> &args);

    static int world(CommandProcessor const *priv, std::vector<std::string> &args);
    static int import(CommandProcessor const *priv, std::vector<std::string> &args);
    static int debug(CommandProcessor const *priv, std::vector<std::string> &args);
    static int quit(CommandProcessor const *priv, std::vector<std::string> &args);
    static int what_is_at(CommandProcessor const *priv, std::vector<std::string> &args);
    static int what_is(CommandProcessor const *priv, std::vector<std::string> &args);
    static int what_is_in(CommandProcessor const *priv, std::vector<std::string> &args);

public:
    CommandProcessor(Gis const*);
    ~CommandProcessor(void);

    friend char* operator>> (char *const in, CommandProcessor &value);
    friend std::ifstream& operator>> (std::ifstream& in, CommandProcessor &value);
    friend std::ofstream& operator<< (std::ofstream& out, CommandProcessor const &value);
};

#endif /* __COMMAND_PROCESSOR_H__ */

