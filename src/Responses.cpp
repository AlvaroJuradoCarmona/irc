#include "Responses.hpp"

std::string NamesReplyResponse::rplNamesReply(const Channel &channel) const {
    const std::vector<User *> users = channel.getUsers();
    
    std::string msg = "= " + channel.getName() + " : ";
    std::vector<User *>::const_iterator it;
    for (it = users.begin(); it != users.end(); it++) {
        const std::string &nickname = (*it)->getNickname();
        if (channel.isOper(nickname))
            msg.append("@");
        msg.append(nickname);
        if (it != users.end() - 1)
            msg.append(" ");
    }
    return msg;
}