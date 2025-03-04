#include "InviteParser.hpp"

/**
 * Parses the INVITE command.
 *
 * Command: INVITE
 * Parameters: <nickname> <channel>
 * 
 * NeedMoreParamsException if the number of arguments is less than the expected.
 */
ACommand *InviteParser::parse(const std::vector<std::string>& tokens) {
    if (tokens.size() < 3)
        throw NeedMoreParamsException(INVITE);
    return new InviteCommand(tokens[1], tokens[2]);
}