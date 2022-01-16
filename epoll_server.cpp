#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <set>
#include <sys/epoll.h>

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

    int epoll_fd = epoll_create(100);
    epoll_event listen_event, events[100];
    listen_event.events = EPOLLIN;
    listen_event.data.fd = listen_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &listen_event) < 0) {
        std::cerr << "Epoll error: " << errno << std::endl;
    }
    std::cout << "Start Serving on Port 12345\n";
    while (true) {
        int n = epoll_wait(epoll_fd, events, 10, -1);
        for (int i = 0; i < n; i ++) {
            auto& event = events[i];
            if (event.data.fd == listen_fd) {
                int worker_fd = accept(listen_fd, nullptr, nullptr);
                if (worker_fd < 0) {
                    std::cerr << "Accept error: " << errno << std::endl;
                } else {
                    epoll_event worker_event;
                    worker_event.events = EPOLLIN;
                    worker_event.data.fd = worker_fd;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, worker_fd, &worker_event) < 0) {
                        std::cerr << "Add worker error: " << errno << std::endl;
                    }
                    std::cout << "Client connected: " << worker_fd << std::endl;
                }
            } else {
                char buf[200] = {};
                size_t n = read(event.data.fd, buf, 100);
                if (n == 0) {
                    std::cout << "Client disconnected: " << event.data.fd << std::endl;
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, nullptr);
                    close(event.data.fd);
                } else if (n < 0) {
                    std::cerr << "Client read error: " << event.data.fd << " " << n << std::endl;
                } else {
                    std::cout << buf << std::endl;
                }
            }
        }
    }
    close(listen_fd);
    return 0;
}