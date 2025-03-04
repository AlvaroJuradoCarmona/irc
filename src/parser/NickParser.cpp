#include "NickParser.hpp"

/**
 * Parses the NICK command.
 * 
 * Command: NICK
 * Parameters: <nickname>
 * 
 * NoNicknameGivenException if the nickname is not specified.
 */
ACommand *NickParser::parse(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2)
        throw NoNicknameGivenException();
    return new NickCommand(tokens[1]);
}