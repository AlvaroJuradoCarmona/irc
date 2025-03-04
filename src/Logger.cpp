#include "Logger.hpp"

Logger::Logger() {}

Logger::~Logger() {}

void Logger::debug(const std::string &message) {
    if (Logger::_debugMode)
        std::cout << "[DEBUG] " << message << '\n';
}