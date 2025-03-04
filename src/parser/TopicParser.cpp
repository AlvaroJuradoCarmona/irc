#include "TopicParser.hpp"

/**
 * Parses the TOPIC command.
 * 
 * Command: TOPIC
 * Parameters: <channel> [<topic>]
 * 
 * NeedMoreParamsException if the number of arguments is less than the expected.
 */
ACommand *TopicParser::parse(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2)
        throw NeedMoreParamsException(TOPIC);
    
    Channel *channel = Server::getInstance().getChannelByName(tokens[1]);

    if (tokens.size() == 2)
        return new TopicCommand(channel);

    return new TopicCommand(channel, tokens[2]);
}