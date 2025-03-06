#ifndef PASS_COMMAND_HPP
# define PASS_COMMAND_HPP

# include "Server.hpp"

class PassCommand : public ACommand {
    private:
        const std::string _password;
    
    public:
        PassCommand(const std::string& password);

        void execute(int clientFd);
};

#endif