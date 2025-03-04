#include "DownCommand.hpp"

DownCommand::DownCommand(const std::string& channelName, const std::string& filename)
    : ACommand(true), _channelName(channelName), _filename(filename) {}

void DownCommand::execute(int clientFd) {
    Server &server = Server::getInstance();
    const User *me = server.getUserByFd(clientFd);

    if (!me->isOnChannel(this->_channelName))
        throw NotOnChannelException(this->_channelName);
    Channel *channel = server.getChannelByName(this->_channelName);
    channel->downloadFile(this->_filename);

    server.sendMessage(clientFd, DownloadResponse(me->getNickname(), this->_filename).getReply());
}