#include "PrivateMessageCommand.hpp"

PrivateMessageCommand::PrivateMessageCommand(const std::vector<std::string> receivers, const std::string &message)
    : ACommand(true), _receivers(receivers), _message(message) {}

void PrivateMessageCommand::execute(int clientFd) {
    Server &server = Server::getInstance();
    const User *sender = server.getUserByFd(clientFd);

    Logger::debug("Sending private message from " + sender->getNickname());
    std::vector<std::string>::const_iterator it;
    for (it = _receivers.begin(); it != _receivers.end(); ++it) {
        try {
            if ((*it)[0] == '#' || (*it)[0] == '&') {
                Logger::debug("Sending private message to channel " + *it);

                Channel *destinationChannel = server.getChannelByName(*it);
                if (!sender->isOnChannel(destinationChannel->getName()))
                    throw NoSuchNickException(*it);

                sender->sendPrivateMessageToChannel(*destinationChannel, _message);
            } else {
                const User *destinationUser = server.getUserByNickname(*it);
                sender->sendPrivateMessageToUser(*destinationUser, _message);
            }
        } catch (const IRCException &e) {
            Logger::debug(*it + " does not exist.");
            server.sendExceptionMessage(clientFd, e);
        }
    }
}