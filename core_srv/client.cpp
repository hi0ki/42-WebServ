#include <iostream>
#include <vector>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>

void client_task(int id, const char* server_ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return;
    }

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        return;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        close(sock);
        return;
    }

    std::string msg = "Hello from client " + std::to_string(id) + "\n";
    send(sock, msg.c_str(), msg.size(), 0);

    char buffer[1024] = {0};
    recv(sock, buffer, sizeof(buffer) - 1, 0);
    std::cout << "[Client " << id << "] Received: " << buffer << std::endl;

    close(sock);
}

int main() {
    const char* server_ip = "127.0.0.1";
    const int port = 8080;
    const int num_clients = 5;

    std::vector<std::thread> threads;

    for (int i = 0; i < num_clients; i++) {
        threads.push_back(std::thread(client_task, i, server_ip, port));
    }

    for (size_t i = 0; i < threads.size(); i++) {
        threads[i].join();
    }

    return 0;
}
