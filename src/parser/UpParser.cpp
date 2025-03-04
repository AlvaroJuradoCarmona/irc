#include "UpParser.hpp"

/**
 * Parses the UP command.
 * 
 * Command: UP
 * Parameters: <channel> <path>
 * 
 * NeedMoreParamsException if the number of arguments is less than the expected.
 * NoSuchChannelException if the channel is not found.
 */
ACommand *UpParser::parse(const std::vector<std::string>& tokens) {
    if (tokens.size() < 3)
        throw NeedMoreParamsException(UP);
    
    if (!Server::getInstance().channelExists(tokens[1]))
        throw NoSuchChannelException(tokens[1]);

    return new UpCommand(tokens[1], tokens[2]);
}