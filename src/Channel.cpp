#include "Channel.hpp"

Channel::Channel()
    : _name(NONE), _password(NONE), _topic(NONE), _limit(NO_LIMIT), _passwordSet(false), _inviteOnly(false) {}

Channel::Channel(const std::string name, User *user)
    : _password(NONE), _topic(NONE), _limit(NO_LIMIT), _passwordSet(false), _inviteOnly(false) {
    if (!checkName(name))
        throw BadChannelMaskException(name);
    this->_name = name;
    this->_operators.push_back(user);
}

Channel::Channel(const Channel &other) {
    *this = other;
}

Channel::~Channel() {
    this->_users.clear();
    this->_operators.clear();
}

Channel &Channel::operator=(const Channel &other) {
    if (this != &other) {
        this->_name = other._name;
        this->_password = other._password;
        this->_users.clear();
        for (size_t i = 0; i < other._users.size(); i++) {
            this->_users.push_back(other._users[i]);
        }
        this->_operators.clear();
        for (size_t i = 0; i < other._operators.size(); i++) {
            this->_operators.push_back(other._operators[i]);
        }
        this->_inviteList.clear();
        for (size_t i = 0; i < other._inviteList.size(); i++) {
            this->_inviteList.push_back(other._inviteList[i]);
        }
        this->_topic = other._topic;
        this->_limit = other._limit;
        this->_passwordSet = other._passwordSet;
        this->_inviteOnly = other._inviteOnly;
        this->_topicProtected = other._topicProtected;
    }
    return *this;
}

bool Channel::checkName(std::string name) const {
    if ((name[0] != '#' && name[0] != '&') || name.size() > MAX_CHANNEL_NAME_LENGTH)
        return false;
    for (size_t i = 1; i < name.size(); i++) {
        if (name[i] == ' ' || name[i] == ',' || name[i] == BELL_CHAR)
            return false;
    }
    return true;
}

std::vector<User *>::iterator Channel::findUser(const std::string &nickname) {
    for (size_t i = 0; i < this->_users.size(); i++) {
        if (this->_users[i]->getNickname() == nickname)
            return this->_users.begin() + i;
    }
    return this->_users.end();
}

std::vector<User *>::const_iterator Channel::findUser(const std::string &nickname) const {
    for (size_t i = 0; i < this->_users.size(); i++) {
        if (this->_users[i]->getNickname() == nickname)
            return this->_users.begin() + i;
    }
    return this->_users.end();
}

std::vector<User *>::iterator Channel::findOper(const std::string &nickname) {
    for (size_t i = 0; i < this->_operators.size(); i++) {
        if (this->_operators[i]->getNickname() == nickname)
            return this->_operators.begin() + i;
    }
    return this->_operators.end();
}

std::vector<User *>::const_iterator Channel::findOper(const std::string &nickname) const {
    for (size_t i = 0; i < this->_operators.size(); i++) {
        if (this->_operators[i]->getNickname() == nickname)
            return this->_operators.begin() + i;
    }
    return this->_operators.end();
}

std::string Channel::getName() const {
    return this->_name;
}

std::string Channel::getPassword() const {
    return this->_password;
}

std::vector<User *> Channel::getUsers() const {
    std::vector<User *> allUsers = this->_operators;
    allUsers.insert(allUsers.end(), this->_users.begin(), this->_users.end());
    return allUsers;
}

std::string Channel::getTopic() const {
    return this->_topic;
}

bool Channel::isPasswordSet() const {
    return this->_passwordSet;
}

void Channel::setPassword(const std::string &password) {
    this->_password = password;
    this->_passwordSet = true;
}

void Channel::unsetPassword() {
    this->_password = NONE;
    this->_passwordSet = false;
}

void Channel::setTopic(std::string topic) {
    this->_topic = topic;
}

bool Channel::checkPassword(std::string password) const {
    return this->_password == password;
}

bool Channel::isUserInvited(std::string nickname) const {
    return std::find(this->_inviteList.begin(), this->_inviteList.end(), nickname) != this->_inviteList.end();
}

void Channel::inviteUser(const std::string &nickname) {
    if (!isUserInvited(nickname))
        this->_inviteList.push_back(nickname);
}

void Channel::setLimit(int limit) {
    this->_limit = limit;
}

bool Channel::hasLimit() const {
    return this->_limit != NO_LIMIT;
}

bool Channel::isFull() const {
    return (int) (this->_users.size() + this->_operators.size()) >= this->_limit && this->_limit != NO_LIMIT;
}

void Channel::addUser(User *user) {
    std::vector<User *>::iterator it = findUser(user->getNickname());
    if (it == this->_users.end())
        this->_users.push_back(user);

}

void Channel::removeUser(const std::string &nickname) {
    Server &server = Server::getInstance();
    
    std::vector<User *>::iterator itUser = this->findUser(nickname);
    std::vector<User *>::iterator itOper = this->findOper(nickname);   
    
    if (itUser == this->_users.end() && itOper == this->_operators.end())
        throw UserNotInChannelException(nickname, this->_name);

    if (itUser != this->_users.end()) {
        (*itUser)->removeChannel(this->_name);
        this->_users.erase(itUser);
    }
    
    if (itOper != this->_operators.end()) {
        (*itOper)->removeChannel(this->_name);
        this->_operators.erase(itOper);
    }

    if (this->_users.empty() && this->_operators.empty()) {
        std::vector<Channel *> &serverChannels = server.getChannels();
        
        for (size_t i = 0; i < serverChannels.size(); i++) {
            if (serverChannels[i]->getName() == this->_name) {
                delete serverChannels[i];
                serverChannels.erase(serverChannels.begin() + i);
                break;
            }
        }
    }
}

void Channel::makeUserAnOper(std::string nickname) {
    std::vector<User *>::iterator it = findUser(nickname);
    if (it != this->_users.end()) {
        this->_operators.push_back(*it);
        this->_users.erase(it);
    }
}

void Channel::makeOperAnUser(std::string nickname) {
    std::vector<User *>::iterator it = findOper(nickname);
    if (it != this->_operators.end()) {
        this->_users.push_back(*it);
        this->_operators.erase(it);
    }
}

bool Channel::isOper(const std::string &nickname) const {
    return findOper(nickname) != this->_operators.end();
}

bool Channel::isInviteOnly() const {
    return this->_inviteOnly;
}

void Channel::setInviteOnly(bool inviteOnly) {
    this->_inviteOnly = inviteOnly;
}

bool Channel::isTopicProtected() const {
    return this->_topicProtected;
}

void Channel::setTopicProtected(bool topicProtected) {
    this->_topicProtected = topicProtected;
}

std::string Channel::getModes() const {
    std::string modes = "";
    if (this->isInviteOnly())
        modes += "i";
    if (this->isTopicProtected())
        modes += "t";
    if (this->isPasswordSet())
        modes += "k";
    if (this->hasLimit())
        modes += "l";
    return modes;
}

std::string Channel::getModeParams() const {
    std::stringstream params;
    if (this->isPasswordSet())
        params << this->_password << " ";
    if (this->hasLimit())
        params << this->_limit << " ";
    // remove the last space
    return params.str().substr(0, params.str().size() - 1);
}

void Channel::broadcast(const std::string &message) {
    Server& server = Server::getInstance();
    std::vector<User *> allUsers = getUsers();
    for (std::vector<User *>::iterator it = allUsers.begin(); it != allUsers.end(); it++)
        server.sendMessage((*it)->getFd(), message);
}

void Channel::uploadFile(const std::string &path) {
    std::ifstream file(path.c_str(), std::ios::binary);

    if (!file)
        throw CannotOpenStreamException();

    std::string filename = path.substr(path.find_last_of("/\\") + 1);

    // Move the read position to the end of the file
    file.seekg(0, std::ios::end);
    // Gets the rea position, which at this point is the size of the file
    std::streamsize size = file.tellg();
    // Move the read position to the beginning of the file
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(&buffer[0], size))
        throw CannotUploadFileException(filename);
    
    this->_files[filename] = buffer;
    buffer.clear();
    file.close();
}

void Channel::downloadFile(const std::string &filename) {
    std::string path = std::string(DOWNLOAD_PATH(this->_name));
    if (_files.find(filename) == _files.end() || mkdir(path.c_str(), FOLDER_PRIVILEGES) == -1)
        throw CannotOpenStreamException();
    std::ofstream file((path + filename).c_str(), std::ios::binary);
    if (!file)
        throw CannotOpenStreamException();

    std::vector<char> buffer = this->_files[filename];
    if (!file.write(&buffer[0], buffer.size()))
        throw CannotDownloadFileException(filename);

    buffer.clear();
    file.close();
}