#ifndef PART_COMMAND_HPP
# define PART_COMMAND_HPP

# include "Server.hpp"

class PartCommand : public ACommand {
    private:
        const std::vector<std::string> _channels;

    public:
        PartCommand(const std::vector<std::string> channels);

        void execute(int clientFd);
};

#endif