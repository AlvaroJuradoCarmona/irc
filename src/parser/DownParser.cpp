#include "DownParser.hpp"

/**
 * Parses the DOWN command.
 * 
 * Command: DOWN
 * Parameters: <channel> <filename>
 * 
 * NeedMoreParamsException if the number of arguments is less than the expected.
 * NoSuchChannelException if the channel is not found.
 */
ACommand *DownParser::parse(const std::vector<std::string>& tokens) {
    if (tokens.size() < 3)
        throw NeedMoreParamsException(DOWN);
    
    if (!Server::getInstance().channelExists(tokens[1]))
        throw NoSuchChannelException(tokens[1]);

    return new DownCommand(tokens[1], tokens[2]);
}