#include <iostream>
#include <cstring>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5813
#define BUF_SIZE 1024

void receive(int client_socket) {
    char buffer[BUF_SIZE];
    while (1) {
        int bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0)
            break;
        buffer[bytes] = '\0';
        std::cout << buffer << std::endl;
    }
}

int main() {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "[error] socket creation failed" << std::endl;
        return 1;
    }

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    if (connect(client_socket, (sockaddr*)&server, sizeof(server)) == -1) {
        std::cerr << "[error] connection failed" << std::endl;
        close(client_socket);
        return 1;
    }

    std::cout << "Connected to server\n";

    std::thread response_thread(receive, client_socket);

    while (1) {
        std::string msg;
        std::cout << "> ";
        std::getline(std::cin, msg);
        send(client_socket, msg.c_str(), msg.size(), 0);
    }

    response_thread.join();
    close(client_socket);
}
