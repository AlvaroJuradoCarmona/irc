#include "ACommand.hpp"

ACommand::ACommand(bool needsValidation) : _needsValidation(needsValidation) {}

ACommand::~ACommand() {}

bool ACommand::needsValidation() const {
    return this->_needsValidation;
}