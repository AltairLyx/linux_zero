#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <netinet/in.h>
#include <string.h>
using namespace std;

int main(int argc, char* argv[]) {
    int listen_fd;
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address{};
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(12345);
    if (bind(listen_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind error: " << errno << std::endl;
        return -1;
    } else {
        std::cout << "Bind success" << std::endl;
    }
    
    if (listen(listen_fd, 2)) {
        std::cerr << "Listen error: " << errno << std::endl;
        return -1;
    } else {
        std::cout << "Listen success" << std::endl;
    }

    int worker_fd = accept(listen_fd, nullptr, nullptr);
    if (worker_fd < 0) {
        std::cerr << "Accept error: " << errno << std::endl;
        return -1;
    } else {
        std::cout << "Accept success" << std::endl;
    }
    char buf[100];
    std::cout << "Start Serving on Port 12345\n";
    while (true) {
        int n = read(worker_fd, (void*)buf, 10);
        if (n == 0) {
            std::cout << "Client disconnected" << std::endl;
            break;
        } else if (n < 0) {
            std::cerr << "Read error: " << n << std::endl;
        } else {
            std::cout << "Read success: " << n << std::endl;
        }
        std::cout << buf << std::endl;
    }
    close(listen_fd);
    close(worker_fd);
    return 0;
}