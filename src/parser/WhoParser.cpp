#include "WhoParser.hpp"

/**
 * Parses the WHO command.
 * 
 * Command: WHO
 * Parameters: [<name> [<o>]]
 */
ACommand *WhoParser::parse(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2)
        return new WhoCommand();
    return new WhoCommand(tokens[1], tokens.size() > 2 && tokens[2] == "o");
}