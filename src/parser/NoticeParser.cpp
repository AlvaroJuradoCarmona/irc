#include "NoticeParser.hpp"

/**
 * Parses the NOTICE command.
 * 
 * Command: NOTICE
 * Parameters: <nickname> <text>
 *
 * NeedMoreParamsException if the number of arguments is less than the expected.
 */
ACommand* NoticeParser::parse(const std::vector<std::string>& tokens) {
    if (tokens.size() < 3)
        throw NeedMoreParamsException(NOTICE);
    return new NoticeCommand(Utils::split(tokens[1], ','), tokens[2]);
}