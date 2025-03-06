#ifndef A_COMMAND_HPP
# define A_COMMAND_HPP

# include <string>

class ACommand {
    protected:
        const bool _needsValidation;
    public:
        ACommand(bool needValidate);
        virtual ~ACommand();

        virtual void execute(int clientFd) = 0;

        bool needsValidation() const;
};

#endif