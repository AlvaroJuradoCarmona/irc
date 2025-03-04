#include "KickParser.hpp"

/**
 * Parses the KICK command.
 * 
 * Command: KICK
 * Parameters: <channel>{,<channel>} <user>{,<user>} [<comment>]
 * 
 * NeedMoreParamsException if the number of arguments is less than the expected.
 * BadChannelMaskException if the channel mask is invalid.
 */
ACommand *KickParser::parse(const std::vector<std::string>& tokens) {
    if (tokens.size() < 3 || tokens[1].empty() || tokens[2].empty())
        throw NeedMoreParamsException(KICK);

    const std::vector<std::string> channels = Utils::split(tokens[1], ',');
    const std::vector<std::string> usersList = Utils::split(tokens[2], ',');
    
    std::vector<std::string>::const_iterator it;
    for (it = channels.begin(); it != channels.end(); ++it) {
        if ((*it)[0] != '#' && (*it)[0] != '&')
            throw BadChannelMaskException(*it);
    }
    return new KickCommand(channels, usersList, tokens.size() > 3 ? tokens[3] : NONE);
}