#include "PassCommand.hpp"

PassCommand::PassCommand(const std::string& password) : ACommand(false), _password(password) {}

void PassCommand::execute(int clientFd) {
    Server& server = Server::getInstance();
	User *user = server.getUserByFd(clientFd);
    if (user->isRegistered())
        throw AlreadyRegisteredException();
    user->setPassword(_password);
}