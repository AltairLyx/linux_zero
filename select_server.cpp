#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <set>

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

    fd_set read_fds;
    std::set<int> worker_fds;
    std::cout << "Start Serving on Port 12345\n";
    while (true) {
        FD_ZERO(&read_fds);
        FD_SET(listen_fd, &read_fds);
        for (auto& worker_fd : worker_fds) {
            FD_SET(worker_fd, &read_fds);
        }
        timeval timeout;
        timeout.tv_sec  = 1;
        timeout.tv_usec = 0;
        // int n = select(5, &read_fds, nullptr, nullptr, &timeout);
        int n = select(7, &read_fds, nullptr, nullptr, nullptr);
        if (n < 0) {
            std::cerr << "Select error: " << errno << std::endl;
        } else if (n == 0) {
            std::cout << "Select timout" << std::endl;
        } else {
            std::cout << "Select success: " << n << std::endl;
        }
        if (FD_ISSET(listen_fd, &read_fds)) {
            int worker_fd = accept(listen_fd, nullptr, nullptr);
            if (worker_fd < 0) {
                std::cerr << "Accept error: " << errno << std::endl;
            } else {
                worker_fds.insert(worker_fd);
                std::cout << "Accept success: " << worker_fd << std::endl;
            }
        }
        std::cout << "Listen complete" << std::endl;
        std::set<int> deleted_fds;
        for (auto& worker_fd : worker_fds) {
            if (!FD_ISSET(worker_fd, &read_fds)) {
                continue;
            }
            char buf[50] = {};
            size_t n = read(worker_fd, buf, 50);
            if (n == 0) {
                std::cout << "Client " << worker_fd << " disconnected" << std::endl;
                deleted_fds.insert(worker_fd);
                close(worker_fd);
            } else if (n < 0) {
                std::cerr << "Read error: " << errno << std::endl;
            }
            std::cout << buf << std::endl;
        }
        for (auto& fd : deleted_fds) {
            worker_fds.erase(fd);
        }
        std::cout << "Select complete" << std::endl;
    }
    close(listen_fd);
    return 0;
}