#include "NoticeCommand.hpp"

NoticeCommand::NoticeCommand(const std::vector<std::string> &receivers, const std::string &message)
    : ACommand(true), _receivers(receivers), _message(message) {}

void NoticeCommand::execute(int clientFd) {
    Server &server = Server::getInstance();
    const User *sender = server.getUserByFd(clientFd);

    for (size_t i = 0; i < _receivers.size(); i++) {
        const std::string recv = _receivers[i];
        try {
            if (recv[0] == '#' || recv[0] == '&') {
                const Channel *destinationChannel = server.getChannelByName(recv);
                if (!sender->isOnChannel(recv))
                    throw IgnoreCommandException();
                sender->sendNoticeToChannel(*destinationChannel, _message);
            } else {
                const User *destinationUser = server.getUserByNickname(recv);
                sender->sendNoticeToUser(*destinationUser, _message);
            }
        } catch (const IRCException &e) {
            Logger::debug("I ignore you, Exception. :D Have a nice day!");
        }
    }
}