#include "QuitParser.hpp"

/**
 * Parses the QUIT command.
 * 
 * Command: QUIT
 * Parameters: [<message>]
 */
ACommand *QuitParser::parse(const std::vector<std::string>& tokens) {
    return new QuitCommand(tokens.size() > 1 ? tokens[1] : NONE);
}