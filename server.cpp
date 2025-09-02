#include <iostream>
#include <thread>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

void handle_client(int client_socket) {
    const char *message = "Hello from server!";
    send(client_socket, message, strlen(message), 0);
    std::cout << "[Server] Sent message: " << message << std::endl;
    close(client_socket);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./server <port>" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Any interface
    address.sin_port = htons(port);

    // Bind
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "[Server] Listening on port " << port << "..." << std::endl;

    std::vector<std::thread> threads;

    while (true) {
        if ((client_socket = accept(server_fd, (struct sockaddr*)&address,
                                    (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        std::cout << "[Server] Client connected." << std::endl;

        // Create a new thread for each client
        threads.emplace_back(handle_client, client_socket);
        threads.back().detach();
    }

    return 0;
}
