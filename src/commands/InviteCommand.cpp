#include "InviteCommand.hpp"

InviteCommand::InviteCommand(const std::string& nickname, const std::string& channelName)
    : ACommand(true), _nickname(nickname), _channelName(channelName) {}

/**
 * Execute the command INVITE.
 * 
 * NoSuchNickException If the nickname is not in use
 * NotOnChannelException If the user which invites is not on the channel
 * NoSuchChannelException If the channel does not exist
 * UserOnChannelException If the user invited is already on the channel
 * ChanOPrivsNeededException If the user which invites is not an operator of the channel and the channel is invite-only
 */
void InviteCommand::execute(int clientFd) {
    Server &server = Server::getInstance();
    const User *me = server.getUserByFd(clientFd);

    if (!server.isNicknameInUse(_nickname))
        throw NoSuchNickException(_nickname);

    if (!server.channelExists(_channelName))
        throw NoSuchChannelException(_channelName);

    if (!me->isOnChannel(_channelName))
        throw NotOnChannelException(_channelName);

    Channel *channel = server.getChannelByName(_channelName);
    const User *invited = server.getUserByNickname(_nickname);

    if (invited->isOnChannel(_channelName))
        throw UserOnChannelException(_nickname, _channelName);

    if (channel->isInviteOnly() && !channel->isOper(me->getNickname()))
        throw ChanOPrivsNeededException(_channelName);

    channel->inviteUser(_nickname);
    server.sendMessage(clientFd, InvitingResponse(_nickname, _channelName).getReply());

    server.sendMessage(server.getUserByNickname(_nickname)->getFd(),
                        CMD_MSG(me->getNickname(), me->getUsername(), me->getHostname(),
                                    INVITE_MSG(_nickname, _channelName)));
}