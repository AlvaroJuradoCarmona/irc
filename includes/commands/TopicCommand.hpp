#ifndef TOPIC_COMMAND_HPP
# define TOPIC_COMMAND_HPP

# include "Channel.hpp"

class Channel;

class TopicCommand : public ACommand {
    private:
        Channel     *_channel;
        const std::string _topic;
        const bool        _newTopicProvided;
    
    public:
        TopicCommand(Channel *channel);
        TopicCommand(Channel *channel, const std::string& topic);

        void execute(int clientFd);
};

#endif