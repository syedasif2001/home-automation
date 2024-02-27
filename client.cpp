#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const int PORT = 5555;

class HomeAutomationClient {
private:
    int client_socket;
    sockaddr_in server_address;
public:
    HomeAutomationClient() {
        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket == -1) {
            std::cerr << "Error creating client socket." << std::endl;
            exit(EXIT_FAILURE);
        }

        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(PORT);

        if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
            std::cerr << "Invalid server address." << std::endl;
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
            std::cerr << "Error connecting to the server." << std::endl;
            close(client_socket);
            exit(EXIT_FAILURE);
        }
    }

    void send_command(const std::string& command) {
        send(client_socket, command.c_str(), command.length(), 0);

        char buffer[1024];
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            std::cerr << "Error receiving response from server." << std::endl;
            return;
        }

        buffer[bytes_received] = '\0';
        std::cout << "Server response: " << buffer << std::endl;
    }

    void Close() {
        close(client_socket);
    }


};

int main() {
    HomeAutomationClient client;

    while (true) {
        std::cout << "Available devices: lamp, fan, tv, refrigerator, air_cooler, washing_machine" << std::endl;
        std::cout << "Enter the device you want to control (or 'exit' to quit): ";
        std::string device;
        std::getline(std::cin, device);

        if (device == "exit") {
            break;
        }

        std::cout << "Enter the action (on/off): ";
        std::string action;
        std::getline(std::cin, action);

        std::string command = device + "," + action;
        client.send_command(command);
    }

    client.Close();

    return 0;
}
