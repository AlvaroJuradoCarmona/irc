#include "ModeCommand.hpp"

ModeCommand::ModeCommand(const bool plus, const std::string& channel, const std::vector<Mode> &modes, const std::vector<std::string>& modeParams) :
    ACommand(true),
    _plus(plus),
    _showChannelModes(false),
    _channel(Server::getInstance().getChannelByName(channel)),
    _modes(modes),
    _modeParams(modeParams) {}

ModeCommand::ModeCommand(const std::string &channel) :
    ACommand(true),
    _plus(false),
    _showChannelModes(true),
    _channel(Server::getInstance().getChannelByName(channel)) {}

/**
 * NotOnChannelException If the user is not on the channel
 * ChanOPrivsNeededException If the user is not an operator of the channel
 * NeedMoreParamsException If the mode needs more parameters
 * UnknownModeException If the mode is unknown
 */
void ModeCommand::execute(int clientFd) {
    Server &server = Server::getInstance();
    const User *me = server.getUserByFd(clientFd);
    if (_showChannelModes) {
        server.sendMessage(
            clientFd,
            ChannelModeIsResponse(me->getNickname(), _channel->getName(), _channel->getModes(), _channel->getModeParams()).getReply()
        );
        return ;
    }

    if (!me->isOnChannel(_channel->getName()))
        throw NotOnChannelException(_channel->getName());

    if (!_channel->isOper(me->getNickname()))
        throw ChanOPrivsNeededException(_channel->getName());

    std::string flag = _plus ? "+" : "-";
    std::vector<std::string>::const_iterator paramIterator = _modeParams.begin();
    std::string modeParams = NONE;
    std::string param = NONE;
    for (size_t i = 0; i < _modes.size(); i++) {
        flag += _modes[i];
        
        if (ModeCommand::modeNeedsParam(_modes[i])) {
            if (paramIterator == _modeParams.end())
                throw NeedMoreParamsException("MODE");
            param = *(paramIterator++);
            modeParams += param + " ";
        } else
            param = NONE;
        try {
            switch (_modes[i]) {
                case INVITE_ONLY:       ModeCommand::inviteOnly(); break;
                case TOPIC_PROTECTED:   ModeCommand::topicProtected(); break;
                case CHANNEL_KEY:       ModeCommand::channelKey(param); break;
                case CHANNEL_OPERATOR:  ModeCommand::channelOperator(param); break;
                case USER_LIMIT:        ModeCommand::userLimit(param); break;
                default:                throw UnknownModeException(std::string(1, _modes[i]));
            }
        } catch (IRCException &e) {
            server.sendExceptionMessage(clientFd, e);
            if (_modes.size() == 1)
                return;
        }
    }
    _channel->broadcast(
        CMD_MSG(me->getNickname(), me->getUsername(), me->getHostname(), MODE_MSG(_channel->getName(), flag, modeParams))
    );
}

void ModeCommand::inviteOnly() {
    _channel->setInviteOnly(_plus);
}

void ModeCommand::topicProtected() {
    _channel->setTopicProtected(_plus);
}

/**
 * KeySetException If the key is already set
 */
void ModeCommand::channelKey(const std::string & param) {
    if (_plus) {
        if (_channel->isPasswordSet())
            throw KeySetException(_channel->getName());
        _channel->setPassword(param);
    } else {
        _channel->unsetPassword();
    }
}

/**
 * UserNotInChannelException If the user is not in the channel
 */
void ModeCommand::channelOperator(const std::string &param) {
    User *user = Server::getInstance().getUserByNickname(param);

    if (!user->isOnChannel(_channel->getName()))
        throw UserNotInChannelException(param, _channel->getName());

    _plus   ? _channel->makeUserAnOper(param)
            : _channel->makeOperAnUser(param);
}

void ModeCommand::userLimit(const std::string & param) {
    if (!_plus) {
        _channel->setLimit(NO_LIMIT);
        return ;
    }

    if (!Utils::isNumber(param))
        return ;

    int numUsers = std::atoi(param.c_str());
    numUsers = std::min(numUsers, MAX_CLIENTS);
    _channel->setLimit(numUsers);
}

bool ModeCommand::modeNeedsParam(Mode mode) const {
    return (_plus && (mode == CHANNEL_KEY || mode == USER_LIMIT))
            || mode == CHANNEL_OPERATOR;
}