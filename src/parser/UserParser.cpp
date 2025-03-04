#include "UserParser.hpp"

/**
 * Parses the USER command.
 * 
 * Command: USER
 * Parameters: <username> <hostname> <server name> :<real name which can contain spaces>
 * 
 * NeedMoreParamsException if the number of arguments is less than the expected.
 */
ACommand *UserParser::parse(const std::vector<std::string>& tokens) {
    if (tokens.size() < 5)
        throw NeedMoreParamsException(USER);

    return new UserCommand(tokens[1], tokens[2], tokens[3], tokens[4]);
}