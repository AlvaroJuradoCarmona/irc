#include "TopicCommand.hpp"

TopicCommand::TopicCommand(Channel *channel) : ACommand(true), _channel(channel), _topic(NONE), _newTopicProvided(false) {}

TopicCommand::TopicCommand(Channel *channel, const std::string& topic)
    : ACommand(true), _channel(channel), _topic(topic), _newTopicProvided(true) {}

void TopicCommand::execute(int clientFd) {
    Server &server = Server::getInstance();
    const User *user = server.getUserByFd(clientFd);

    const std::string &channelName = _channel->getName();
    const std::string &nickname = user->getNickname();

    if (!user->isOnChannel(channelName))
        throw NotOnChannelException(channelName);
    Logger::debug("User " + nickname + " in channel " + channelName);

    if (_newTopicProvided && _channel->isTopicProtected() && !_channel->isOper(nickname))
        throw ChanOPrivsNeededException(channelName);

    if (_newTopicProvided) {
        _channel->setTopic(_topic);
        Logger::debug("Sending the new topic to all its users");
        
        _channel->broadcast(
            CMD_MSG(nickname, user->getUsername(), user->getHostname(),
                TOPIC_MSG(channelName, _topic)
            )
        );
    } else {
        const std::string &channelTopic = _channel->getTopic();
        const std::string &message = channelTopic.empty()
            ? NoTopicResponse(nickname, channelName).getReply()
            : TopicResponse(nickname, channelName, channelTopic).getReply();
        Logger::debug("Sending topic to user " + nickname);
        server.sendMessage(clientFd, message);
    }
    Logger::debug("Channel's topic is: " + _channel->getTopic());
}