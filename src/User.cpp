#include "User.hpp"

User::User(int fd) : _fd(fd), _registered(false) {}

User::User(const User &other) {
    *this = other;
}

User::~User() {}

User &User::operator=(const User &other) {
    if (this != &other) {
        _fd = other.getFd();
        _registered = other.isRegistered();
        _username = other.getUsername();
        _hostname = other.getHostname();
        _serverName = other._serverName;
        _realName = other._realName;
        _nickname = other.getNickname();
        _password = other._password;
        _channels.clear();
        for (size_t i = 0; i < other._channels.size(); i++)
            _channels.push_back(other._channels[i]);
    }
    return *this;
}

bool User::operator<(const User& other) const {
    return _nickname < other.getNickname();
}

std::vector<Channel *>::const_iterator User::findChannel(const std::string &channelName) const {
    for (size_t i = 0; i < _channels.size(); i++) {
        if (_channels[i]->getName() == channelName)
            return _channels.begin() + i;
    }
    return _channels.end();
}

std::vector<Channel *>::iterator User::findChannel(const std::string &channelName) {
    for (size_t i = 0; i < _channels.size(); i++) {
        if (_channels[i]->getName() == channelName)
            return _channels.begin() + i;
    }
    return _channels.end();
}

bool User::isUserInMaxChannels() const {
    return _channels.size() >= MAX_CHANNELS;
}

bool User::isOnChannel(const std::string &channelName) const {
    std::vector<Channel *>::const_iterator it = findChannel(channelName);
    return it != _channels.end();
}

int User::getFd() const {
    return _fd;
}

std::string User::getUsername() const {
    return _username;
}

void User::setUsername(const std::string& username) {
    _username = username;
}

std::string User::getHostname() const {
    return _hostname;
}

std::string User::getRealName() const {
    return _realName;
}

void User::setHostname(const std::string& hostname) {
    _hostname = hostname;
}

void User::setServerName(const std::string& serverName) {
    _serverName = serverName;
}

void User::setRealName(const std::string& realName) {
    _realName = realName;
}

std::string User::getNickname() const {
    return _nickname;
}

void User::setNickname(const std::string& nickname) {
    _nickname = nickname;
}

void User::setPassword(const std::string& password) {
    _password = password;
}


bool User::canRegister() const {
    return  !(_username.empty()
            || _hostname.empty()
            || _serverName.empty()
            || _realName.empty()
            || _nickname.empty());
}

void User::makeRegistration() {
    Server &server = Server::getInstance();

    if (!server.isValidPassword(_password))
        throw PasswordMismatchException();

    server.sendMessage(getFd(), WelcomeResponse(_nickname, _username, _hostname).getReply());
    server.sendMessage(getFd(), YourHostResponse(_nickname, _serverName).getReply());
    server.sendMessage(getFd(), CreatedResponse(_nickname, server.getCreationDate()).getReply());
    server.sendMessage(getFd(), MyInfoResponse(_nickname, _serverName).getReply());

    _registered = true;
}

bool User::isRegistered() const {
    return _registered;
}

void User::addChannel(Channel *channel) {
    _channels.push_back(channel);
}

void User::removeChannel(const std::string &channelName) {
    std::vector<Channel *>::iterator channel = findChannel(channelName);
    if (channel != _channels.end())
        _channels.erase(channel);
}

void User::sendPrivateMessageToUser(const User &destination, const std::string& message) const {
    Logger::debug("Sending private message to " + destination.getNickname() + " from " + getNickname() + ": " + message);
    std::string response = CMD_MSG(_nickname, _username, _hostname, PRIVMSG_MSG(destination.getNickname(), message));
    Server::getInstance().sendMessage(destination.getFd(), response);
}

void User::sendNoticeToUser(const User &destination, const std::string& message) const {
    Logger::debug("Sending NOTICE to " + destination.getNickname() + " from " + getNickname() + ": " + message);
    std::string response = CMD_MSG(_nickname, _username, _hostname, NOTICE_MSG(destination.getNickname(), message));
    Server::getInstance().sendMessage(destination.getFd(), response);
}

Channel *User::getChannelByName(std::string &channelName) {
    std::vector<Channel *>::iterator it = findChannel(channelName);
    return (it != _channels.end()) ? *it : NULL;
}

void User::sendPrivateMessageToChannel(const Channel &destination, const std::string& message) const {
    Logger::debug("Sending private message to channel " + destination.getName() + " from " + getNickname() + ": " + message);
    std::string response = CMD_MSG(_nickname, _username, _hostname, PRIVMSG_MSG(destination.getName(), message));
    std::vector<User *> users = destination.getUsers();
    for (size_t i = 0; i < users.size(); i++)
        if (users[i]->getNickname() != _nickname)
            Server::getInstance().sendMessage(users[i]->getFd(), response);
}

void User::sendNoticeToChannel(const Channel &destination, const std::string& message) const {
    Logger::debug("Sending private message to channel " + destination.getName() + " from " + getNickname() + ": " + message);
    std::string response = CMD_MSG(_nickname, _username, _hostname, NOTICE_MSG(destination.getName(), message));
    std::vector<User *> users = destination.getUsers();
    for (size_t i = 0; i < users.size(); i++)
        if (users[i]->getNickname() != _nickname)
            Server::getInstance().sendMessage(users[i]->getFd(), response);
}

std::vector<Channel *> User::getChannels() const {
    return _channels;
}