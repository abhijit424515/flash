#include <iostream>
#include <thread>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 5813
#define BUF_SIZE 1024

int pipe_fds[2];

void debug_stdin() {
    char stdin_buffer[BUF_SIZE];
    while (1) {
        ssize_t bytes = read(STDIN_FILENO, stdin_buffer, sizeof(stdin_buffer) - 1);
        if (bytes > 0) {
            stdin_buffer[bytes] = '\0';
            std::cout << "[debug] stdin: " << stdin_buffer;
        } else if (bytes == 0) {
            // End of input (pipe closed)
            std::cerr << "[debug] End of stdin" << std::endl;
            break;
        } else {
            std::cerr << "[error] Reading from stdin failed" << std::endl;
            break;
        }
    }
}

void handle_client(int client_socket) {
    char buffer[BUF_SIZE];
    while (1) {
        int bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            close(client_socket);
            break;
        }
        buffer[bytes] = '\0';
		if (buffer[bytes - 1] != '\n') {
            buffer[bytes] = '\n';
            buffer[++bytes] = '\0';
        }

		if (write(pipe_fds[1], buffer, bytes) == -1) {
            std::cerr << "[error] write failed" << std::endl;
            break;
        }
    }
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "[error] socket creation failed" << std::endl;
        return 1;
    }

	int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "[error] setsockopt failed" << std::endl;
        close(server_fd);
        return 1;
    }

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(server_fd, (sockaddr*)&server, sizeof(server)) == -1) {
        std::cerr << "[error] bind failed" << std::endl;
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 5) == -1) {
        std::cerr << "[error] listen failed" << std::endl;
        close(server_fd);
        return 1;
    }

	if (pipe(pipe_fds) == -1) {
		std::cerr << "[error] pipe failed" << std::endl;
		close(server_fd);
		return 1;
	}
	if (dup2(pipe_fds[0], STDIN_FILENO) == -1) {
		std::cerr << "[error] dup2 failed" << std::endl;
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        close(server_fd);
        return 1;
    }
	close(pipe_fds[0]);

	std::thread debug_thread(debug_stdin);

    std::vector<std::thread> client_threads;
    while (1) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd == -1) {
            std::cerr << "[error] accept failed" << std::endl;
			close(client_fd);
            continue;
        }

        client_threads.emplace_back(handle_client, client_fd);
    }

    for (auto& t : client_threads)
        if (t.joinable()) 
			t.join();

	if (debug_thread.joinable())
        debug_thread.join();

	close(pipe_fds[0]);
    close(server_fd);
    return 0;
}
