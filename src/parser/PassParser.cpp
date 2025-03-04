#include "PassParser.hpp"

/**
 * Parses the PASS command.
 * 
 * Command: PASS
 * Parameters: <password>
 * 
 * NeedMoreParamsException if the number of arguments is less than the expected.
 */
ACommand* PassParser::parse(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2)
        throw NeedMoreParamsException(PASS);
    return new PassCommand(tokens[1]);
}