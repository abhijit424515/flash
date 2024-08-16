#include "common.hh"

int main() {
	struct sockaddr_in serv_addr;

	int sock;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        cerr << "Invalid address/ Address not supported" << endl;
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "Connection Failed" << endl;
        exit(EXIT_FAILURE);
    }

	string message = "Hello from client";
	send(sock, message.c_str(), message.size(), 0);

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

	cout << "Received " << buffer->size()+1 << " bytes" << endl;
	cout << "Message: " << *buffer << endl;

    close(sock);
}