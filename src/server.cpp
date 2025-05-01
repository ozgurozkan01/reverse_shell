#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

// Link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "4444"

void init_windows_socket()
{
    WSADATA wsa_data;
    // Techically after this line is run, wsa_data will store the version variable (2.2)
    int i_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);

    // Remember in C++ 0 means no error, other values mean error.
    if (i_result != 0)
    {
        std::cerr << "WSAStartup could be started properly !!\n";
        exit(1);
    }

}

SOCKET create_socket()
{
    struct addrinfo *result = nullptr;
    struct addrinfo hints = {};

    memset(&hints, 0, sizeof(hints));

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; // ipV4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE; // mean Listener not Connector

    int i_result = getaddrinfo(
            nullptr,            // can be bind to all local ips
            DEFAULT_PORT,       // bind over this port
            &hints,             // while address resolving, use features of hint
            &result             // start point of linked list which stands for as a container stores the results.
            );

    if (i_result != 0)
    {
        std::cerr << "getaddrinfo failed: " << i_result << std::endl;
        exit(1);
    }

    SOCKET server_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (server_socket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation was failed !!\n";
        exit(1);
    }

    i_result = bind(server_socket, result->ai_addr, (int)result->ai_addrlen);

    if (i_result == SOCKET_ERROR)
    {
        std::cerr << "Binding was failed !! \n";
        exit(1);
    }

    return server_socket;
}

void listen_connection(SOCKET server_socket)
{
    int i_result = listen(server_socket, SOMAXCONN);

    if (i_result == SOCKET_ERROR)
    {
        std::cerr << "Listening was failed !!\n";
        exit(1);
    }

    std::cout << "Waiting for connection...\n";
}

SOCKET accept_connection(SOCKET server_socket)
{
    SOCKET client_socket = accept(server_socket, nullptr, nullptr);

    if (client_socket == INVALID_SOCKET)
    {
        std::cerr << "Connection acception was failed !!\n";
        exit(1);
    }

    std::cout << "Client connected succesfully !!\n";

    return client_socket;
}

void send_message(SOCKET client_socket, const std::string& message)
{
    int i_result = send(client_socket, message.c_str(), message.length(), 0);

    if (i_result == SOCKET_ERROR)
    {
        std::cerr << "Message sending was failed !!\n";
        exit(1);
    }

    std::cout << "Message sent to client succesfully.\n";
}

int main()
{
    init_windows_socket();

    SOCKET server_socket = create_socket();
    listen_connection(server_socket);

    SOCKET client_socket = accept_connection(server_socket);
    std::string message = "Connection established.";

    send_message(client_socket, message);

    return 0;
}
