#include "PrivateMessageParser.hpp"

/**
 * Parses the PRIVMSG command.
 * 
 * Command: PRIVMSG
 * Parameters: <receiver>{,<receiver>} :<text to be sent>
 * 
 * NoRecipientGivenException if no recipient is given.
 * NoTextToSendException if no text is given.
 */
ACommand* PrivateMessageParser::parse(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2)
        throw NoRecipientGivenException(PRIVMSG);

    if (tokens.size() < 3)
        throw NoTextToSendException();

    return new PrivateMessageCommand(Utils::split(tokens[1], ','), tokens[2]);
}