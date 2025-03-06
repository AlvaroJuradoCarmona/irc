#ifndef QUIT_COMMAND_HPP
# define QUIT_COMMAND_HPP

# include <set>

# include "Server.hpp"

class QuitCommand : public ACommand {
    private:
        const std::string _message;
    public:
        QuitCommand(const std::string &message);

        void execute(int clientFd);
};

#endif