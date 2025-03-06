#include "Server.hpp"

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << INVALID_ARGS << std::endl;
        return 1;
    }

    try {
        const std::string port = std::string(argv[1]);
        const std::string password = std::string(argv[2]);
        Server::init(port, password);
    } catch (ServerException &e) {
        std::cerr << e.what() << '\n';
    } catch (IRCException &e) {
        std::cerr << e.what() << '\n';
    }
    Server::deleteServer();
    return 0;
}