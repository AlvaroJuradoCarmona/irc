#include "UserCommand.hpp"

UserCommand::UserCommand(const std::string& username, const std::string& hostname, const std::string& serverName,
                            const std::string& realName)
    : ACommand(false), _username(username), _hostname(hostname),
                        _serverName(serverName), _realName(realName) {}

void UserCommand::execute(int clientFd) {
    Server& server = Server::getInstance();
    User *user = server.getUserByFd(clientFd);

    if (user->isRegistered() && !user->getUsername().empty())
        throw AlreadyRegisteredException();
    user->setUsername(_username);
    user->setHostname(_hostname);
    user->setServerName(_serverName);
    user->setRealName(_realName);
    if (user->canRegister())
        user->makeRegistration();
}