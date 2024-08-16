#include "common.hh"
#include "threadpool.hh"

int server_fd;

void handle_signal(int signal) {
	close(server_fd);
	exit(EXIT_SUCCESS);
}

void handle_client(int sock) {
	// Read from the socket
	string *buffer = new string(DEFAULT_BUFFER_SIZE, '\0');
	ssize_t bytes_received;
	while ((bytes_received = recv(sock, buffer->data(), buffer->size(), 0)) > 0) {
		if (bytes_received == buffer->size())
			buffer->resize(buffer->size()*2);

		if (buffer->back() == 0) {
			buffer->pop_back();
			break;
		}
	}
	if (bytes_received < 0) {
		perror("recv failed");
		exit(EXIT_FAILURE);
	}

	// Display the message
	cout << "Received " << buffer->size()+1 << " bytes." << endl;
	cout << "Message: " << *buffer << endl;

	// Send a response
	string message = "Hello from server";
	send(sock, message.c_str(), message.size(), 0);
	close(sock);
}

int main() {
	ThreadPool pool(4);
	signal(SIGINT, handle_signal); 

	struct sockaddr_in addr;
	int addrlen = sizeof(addr);

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Attach socket to PORT
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt failed");
		exit(EXIT_FAILURE);
	}
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(PORT);

	// Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
	
	if (listen(server_fd, BACKLOG) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

	while (1) {
		// Wait for a connection, then accept it
		int new_socket;
		if ((new_socket = accept(server_fd, (struct sockaddr *)&addr, (socklen_t*)&addrlen)) < 0) {
			perror("accept");
			close(server_fd);
			exit(EXIT_FAILURE);
		}
		
		// Handle the client in a separate thread
		pool.enqueue(handle_client, new_socket);
	}

    close(server_fd);
}