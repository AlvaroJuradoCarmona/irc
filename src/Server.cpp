#include "Server.hpp"

Server *Server::_server = NULL;

bool Server::isValidPort(const std::string &port) const {
    for (size_t i = 0; i < port.length(); i++) {
        if (!std::isdigit(port[i]))
            return false;
    }
    int portNum = std::atoi(port.c_str());
    return portNum >= MIN_PORT && portNum <= MAX_PORT;
}

Server::Server(const std::string port, const std::string password) : _password(password), _signalReceived(false) {
    if (!isValidPort(port))
        throw ServerException(PORT_OUT_OF_RANGE_ERR);
    _port = std::atoi(port.c_str());
    generateDate();
    initServer();
}

Server::~Server() {
    closeConnections();
    for (size_t i = 0; i < _users.size(); i++)
        delete _users[i];
    for (size_t i = 0; i < _channels.size(); i++)
        delete _channels[i];
}

void Server::deleteServer() {
    delete _server;
}

void Server::init(std::string port, std::string password) {
    Server::_server = new Server(port, password);
    signal(SIGINT, &signalHandler);
    signal(SIGQUIT, &signalHandler);
    Server::_server->listenClients();
}

Server &Server::getInstance() {
    if (Server::_server == NULL)
        Server::_server = new Server(DEFAULT_PORT, DEFAULT_PASS);
    return *Server::_server;
}

void signalHandler(int signal) {
    (void) signal;
    std::cout << "\b\b\033[K";
    Logger::debug("Signal Received!!");
    Logger::debug("Stopping the server...");
    Server::getInstance().setSignalReceived();
}

void Server::closeConnections() {
    for (size_t i = 0; i < _users.size(); i++)
        close(_users[i]->getFd());

    if (_socketFd != -1)
        close(_socketFd);
}

void Server::initServer() {
    _socketFd = socket(
        AF_INET,        // IPv4
        SOCK_STREAM,    // Stream socket
        0               // Let system decide the best protocol (SOCK_STREAM => TCP)
    );

    if (_socketFd < 0)
        throw ServerException(SOCKET_EXPT);

    // Config server address and port
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_addr.s_addr = INADDR_ANY;
    // htons: Converts host format port to network format port.
    _serverAddr.sin_port = htons(_port);

    // set the socket option (SO_REUSEADDR) to reuse the address
    int enabled = 1;
    if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(int)) < 0)
        throw ServerException(REUSE_ADDR_EXPT);

    // Setting the socket option for non-blocking socket (O_NONBLOCK)
    if (fcntl(_socketFd, F_SETFL, O_NONBLOCK) < 0)
        throw ServerException(FCNTL_EXPT);

    // Binding the server socket
    if (bind(_socketFd, (struct sockaddr*)&_serverAddr, sizeof(_serverAddr)) < 0)
        throw ServerException(BIND_EXPT);

    // Listen to incoming connections
    if (listen(_socketFd, MAX_CLIENTS) < 0)
        throw ServerException(LISTEN_EXPT);

    // Configure the first element of the pollfd structure array for the server socket
    struct pollfd socketPoll;
    
    socketPoll.fd = _socketFd;
    socketPoll.events = POLLIN;
    _fds.push_back(socketPoll);
}

void Server::listenClients() {
    while (!_signalReceived) {
        if (poll(&_fds[0], _fds.size(), -1) == -1 && !_signalReceived)
            throw ServerException(POLL_EXPT);

        for (size_t i = 0; i < _fds.size(); i++) {
            if (_fds[i].revents == 0)
                continue;

            // Client disconnected
            if (_fds[i].revents & POLLHUP || _fds[i].revents & POLLERR || _fds[i].revents & POLLNVAL) {
                handleClientDisconnection(_fds[i].fd);
                continue;
            }

            if (_fds[i].revents != POLLIN)
                throw ServerException(REVENTS_EXPT);
            if (_fds[i].fd == _socketFd)
                handleNewConnection();
            else
                handleExistingConnection(_fds[i].fd);
        }
    }
    Logger::debug("Closing connections...");
}

void Server::handleClientDisconnection(int clientFd) {
    std::vector<User *>::iterator userIt = findUserByFd(clientFd);

    if (userIt == _users.end())
        return;
    
    // Delete user from all channels where it joined
    std::vector<Channel *>::iterator channelIt = _channels.begin();
    while (channelIt != _channels.end() && !(*userIt)->isOnChannel((*channelIt)->getName()))
        ++channelIt;
    if (channelIt != _channels.end())
        (*channelIt)->removeUser((*userIt)->getNickname());

    // Erase possible input left by the user
    if (_inputBuffer.find(clientFd) != _inputBuffer.end())
        _inputBuffer.erase(clientFd);

    // Remove user from the server vector
    delete *userIt;
    _users.erase(userIt);

    // Close clientFd and remove it from the poll_fds array
    if (clientFd != -1) {
        std::vector<struct pollfd>::iterator pollIt = _fds.begin();
        while (pollIt != _fds.end() && pollIt->fd != clientFd)
            ++pollIt;
        if (pollIt != _fds.end()) {
            close(pollIt->fd);
            _fds.erase(pollIt);
        }
    }
}

void Server::handleNewConnection() {
    
    // Accept a new connection
    socklen_t size = sizeof(_serverAddr);
    int clientSocket = accept(_socketFd, (struct sockaddr*) &_serverAddr, &size);

    if (clientSocket < 0)
        throw ServerException(ACCEPT_EXPT);
    
    // Setting the client socket option for non-blocking socket (O_NONBLOCK)
    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0)
        throw ServerException(FCNTL_EXPT);

    _users.push_back(new User(clientSocket));
    // Add new socket to poll_fds array
    struct pollfd newPoll;
    newPoll.fd = clientSocket;
    newPoll.events = POLLIN;
    _fds.push_back(newPoll);
}

void Server::handleExistingConnection(int clientFd) {
    char buffer[BUFFER_SIZE];
    std::memset(buffer, '\0', BUFFER_SIZE);

    int readBytes = recv(clientFd, buffer, BUFFER_SIZE, 0);
    if (readBytes < 0) {
        handleClientDisconnection(clientFd);
        Logger::debug(RECV_EXPT);
    } else if (readBytes == 0) {
        QuitCommand quit("connection closed");
        quit.execute(clientFd);
        return;
    }
    
    buffer[readBytes] = '\0';

    if (!buffer[0])
        return;

    if (_inputBuffer.find(clientFd) == _inputBuffer.end())
        _inputBuffer[clientFd] = "";

    _inputBuffer[clientFd] += std::string(buffer, readBytes);

    Logger::debug("Mensaje del cliente: " + _inputBuffer[clientFd]);
    size_t pos;
    while ((pos = _inputBuffer[clientFd].find_first_of("\r\n")) != std::string::npos) {
        std::string message = _inputBuffer[clientFd].substr(0, pos);
        _inputBuffer[clientFd] = _inputBuffer[clientFd].substr(pos + 1);
        
        if (!_inputBuffer[clientFd].empty() && _inputBuffer[clientFd][0] == '\n') {
            _inputBuffer[clientFd] = _inputBuffer[clientFd].substr(1);
        }

        User *client = getUserByFd(clientFd);

        try {
            ACommand* command = CommandParser::parse(message, client);

            if (command->needsValidation() && !client->isRegistered())
                throw NotRegisteredException();
            command->execute(clientFd);
        } catch (PasswordMismatchException &e) {
            sendExceptionMessage(clientFd, e);
            handleClientDisconnection(clientFd);
        } catch (IRCException &e) {
            sendExceptionMessage(clientFd, e);
        } catch (IgnoreCommandException &e) {
            Logger::debug("Ignoring command");
        }
    }
}

bool Server::isValidPassword(const std::string &password) const {
    return password == _password;
}

void Server::setSignalReceived() {
    _signalReceived = true;
}

User *Server::getUserByFd(int clientFd) {
    return *findUserByFd(clientFd);
}

const User *Server::getUserByFd(int clientFd) const {
    return *findUserByFd(clientFd);
}

bool Server::isNicknameInUse(const std::string& nickname) const {
    std::vector<User *>::const_iterator it = findUserByNickname(nickname);
    return it != _users.end();
}

User *Server::getUserByNickname(const std::string &nickname) {
    std::vector<User *>::iterator it = findUserByNickname(nickname);
    if (it == _users.end())
        throw NoSuchNickException(nickname);
    return *it;
}

void Server::sendMessage(int clientFd, const std::string& message) {
    if (!isUserConnected(clientFd))
        return;

    int msgSignal = 0;
    std::string messageToSend = message + std::string("\r\n");
    
    // setsocketopt in Mac (+ 0 value on send function) to avoid sending signal SIGPIPE
    // Same behaviour in Linux with MSG_NOSIGNAL on send function
    #ifdef __APPLE__
        int enabled = 1;
        setsockopt(clientFd, SOL_SOCKET, SO_NOSIGPIPE, (void *) &enabled, sizeof(int));
    #else
        msgSignal = MSG_NOSIGNAL;
    #endif
    if (send(clientFd, messageToSend.c_str(), messageToSend.size(), msgSignal) < 0) {
        handleClientDisconnection(clientFd);
        Logger::debug(SEND_EXPT);
    }
}

void Server::sendExceptionMessage(int clientFd, const IRCException &e) {
    std::string clientNickname = getUserByFd(clientFd)->getNickname();
    sendMessage(clientFd, RESPONSE_MSG(e.getErrorCode(), clientNickname.empty() ? "*" : clientNickname, e.what()));
}

void Server::removeUser(int fd) {
    std::vector<User *>::iterator it = findUserByFd(fd);
    if (it != _users.end()) {
        close((*it)->getFd());
        _users.erase(it);
    }
}

std::vector<User *> Server::getUsers() {
    return _users;
}

std::vector<User *>::iterator Server::findUserByFd(int clientFd) {
    for (size_t i = 0; i < _users.size(); i++) {
        if (_users[i]->getFd() == clientFd)
            return _users.begin() + i;
    }
    return _users.end();
}

std::vector<User *>::const_iterator Server::findUserByFd(int clientFd) const {
    for (size_t i = 0; i < _users.size(); i++) {
        if (_users[i]->getFd() == clientFd)
            return _users.begin() + i;
    }
    return _users.end();
}

std::vector<User *>::iterator Server::findUserByNickname(const std::string &nickname) {
    for (size_t i = 0; i < _users.size(); i++) {
        if (_users[i]->getNickname() == nickname)
            return _users.begin() + i;
    }
    return _users.end();
}

std::vector<User *>::const_iterator Server::findUserByNickname(const std::string &nickname) const {
    for (size_t i = 0; i < _users.size(); i++) {
        if (_users[i]->getNickname() == nickname)
            return _users.begin() + i;
    }
    return _users.end();
}

std::vector<Channel *>::iterator Server::findChannel(const std::string &channelName) {
    for (size_t i = 0; i < _channels.size(); i++) {
        if (_channels[i]->getName() == channelName)
            return _channels.begin() + i;
    }
    return _channels.end();
}

std::vector<Channel *>::const_iterator Server::findChannel(const std::string &channelName) const {
    for (size_t i = 0; i < _channels.size(); i++) {
        if (_channels[i]->getName() == channelName)
            return _channels.begin() + i;
    }
    return _channels.end();
}

void Server::addChannel(Channel *channel) {
    std::vector<Channel *>::iterator it = findChannel(channel->getName());
    if (it == _channels.end())
        _channels.push_back(channel);
}

std::vector<Channel *> &Server::getChannels() {
    return _channels;
}

void Server::removeChannel(std::string channelName) {
    std::vector<Channel *>::iterator it = findChannel(channelName);
    if (it != _channels.end())
        _channels.erase(it);
}

Channel *Server::getChannelByName(const std::string &channelName) {
    std::vector<Channel *>::iterator it = findChannel(channelName);
    if (it == _channels.end())
        throw NoSuchChannelException(channelName);
    return *it;
}

bool Server::channelExists(const std::string &channelName) const {
    return findChannel(channelName) != _channels.end();
}

bool Server::isUserConnected(int clientFd) const {
    for (size_t i = 0; i < _fds.size(); i++) {
        if (_fds[i].fd == clientFd) {
            return true;
        }
    }
    return false;
}

void Server::generateDate() {
    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);
    char buffer[100];

    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S UTC", now);
    _creationDate = std::string(buffer);
}

std::string Server::getCreationDate() const {
    return _creationDate;
}