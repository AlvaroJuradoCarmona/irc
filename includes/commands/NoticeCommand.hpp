#ifndef NOTICE_COMMAND_HPP
# define NOTICE_COMMAND_HPP

# include "Server.hpp"

class NoticeCommand : public ACommand {
    private:
        const std::vector<std::string> _receivers;
        const std::string _message;
    public:
        NoticeCommand(const std::vector<std::string> &receivers, const std::string &message);
        void execute(int clientFd);
};

#endif