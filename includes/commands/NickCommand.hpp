#ifndef NICK_COMMAND_HPP
# define NICK_COMMAND_HPP

# include "Server.hpp"

# define MAX_NICKNAME_SIZE 9

class NickCommand : public ACommand {
    private:
        const std::string _nickname;

        bool isValidNickname() const;
    public:
        NickCommand(const std::string& nickname);

        void execute(int clientFd);
};

#endif