#include "PartParser.hpp"

/**
 * Parses the PART command.
 * 
 * Command: PART
 * Parameters: <channel>{,<channel>}
 * 
 * NeedMoreParamsException if the number of arguments is less than the expected.
 * BadChannelMaskException if the channel mask is invalid.
 */
ACommand *PartParser::parse(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2)
        throw NeedMoreParamsException(PART);

    std::vector<std::string> channels = Utils::split(tokens[1], ',');

    std::vector<std::string>::const_iterator it;
    for (it = channels.begin(); it != channels.end(); ++it) {
        if ((*it)[0] != '#' && (*it)[0] != '&')
            throw BadChannelMaskException(*it);
    }
    return new PartCommand(channels);
}