#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    int connect_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect_fd < 0) {
        std::cerr << "Socket create error" << std::endl;
        return -1;
    }
    sockaddr_in address{};
    // memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = htons(12345);
    if (argc > 1) {
        std::cout << "Connect to: " << argv[1];
        address.sin_port = htons(std::atoi(argv[1]));
    }
    if (connect(connect_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Connect error: " << errno << std::endl;
        return -1;
    } else {
        std::cout << "Connect success" << std::endl;
    }
    std::string s;
    while (std::cin >> s) {
        int n = write(connect_fd, s.c_str(), s.length());
        if (n < 0) {
            std::cerr << "Write error: " << n << std::endl;
        } else {
            std::cout << "Write success: " << n << std::endl;
        }
    }
    if (close(connect_fd) < 0) {
        std::cerr << "close error: " << errno << std::endl;
    } else {
        std::cout << "Connect closed" << std::endl;
    }
    return 0;
}