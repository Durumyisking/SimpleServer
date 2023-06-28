#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <thread>

#define DEFAULT_PORT   5150
#define DEFAULT_BUFFER 2048

void ErrorHandling(const std::wstring& message);
void HandleClient(SOCKET clientSocket);

int main()
{
    WSADATA wsd;
    SOCKET serverSockets; // �������� : Ŭ���̾�Ʈ ���� ���� �� Ŭ���̾�Ʈ ���� ���� ���  
    std::vector<SOCKET> clientSockets; // Ŭ���̾�Ʈ ���ϵ��� ��� ����

    struct sockaddr_in localAddr; // ���������� ���ε� �� �����ּ�

    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
    {
        ErrorHandling(L"WSAStartup() error!");
    }
    else
    {
        puts("The library has been initialized.\n\n");
    }

    serverSockets = socket(AF_INET, SOCK_STREAM, 0); // ���� ���� ����
    if (serverSockets == INVALID_SOCKET)
    {
        ErrorHandling(L"socket() error!");
    }
    else
    {
        std::cout << "Server socket created.\n" << std::endl;
    }

    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY); // ������ �ּ� ���� (INADDR_ANY Ű����� ��� ��Ʈ��ũ �������̽����� ������ ������ ����)
    localAddr.sin_port = htons(DEFAULT_PORT); // ��Ʈ�ѹ� ����
    localAddr.sin_family = AF_INET; // IPv4�� �ּ� ü�� ����

    if (bind(serverSockets, (struct sockaddr*)&localAddr, sizeof(localAddr)) == SOCKET_ERROR)
    {
        ErrorHandling(L"bind() error!");
    }

    listen(serverSockets, SOMAXCONN);

    char serverIP[20] = { 0 };
    if (NULL == inet_ntop(AF_INET, &localAddr.sin_addr, serverIP, sizeof(serverIP))) // inet_ntop ip �ּҸ� ���ڿ� ���·� ��ȯ
    {
        ErrorHandling(L"inet_ntop() error!");
    }
    else
    {
        std::cout << "[Server IP: " << serverIP << "::" << ntohs(localAddr.sin_port) << "]" << std::endl;
    }

    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        std::cout << "Failed to get hostname" << std::endl;
        WSACleanup();
        return 1;
    }

    struct addrinfo* ipAddr = nullptr;
    struct addrinfo hints {};
    hints.ai_family = AF_INET;

    if (getaddrinfo(hostname, nullptr, &hints, &ipAddr) != 0) {
        std::cout << "Failed to get IP address" << std::endl;
        WSACleanup();
        return 1;
    }

    struct sockaddr_in* addr = reinterpret_cast<struct sockaddr_in*>(ipAddr->ai_addr);
    char ipAddress[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &(addr->sin_addr), ipAddress, INET_ADDRSTRLEN) == nullptr) {
        std::cout << "Failed to convert IP address to string" << std::endl;
        WSACleanup();
        return 1;
    }

    std::cout << "IP address: " << ipAddress << std::endl;

    freeaddrinfo(ipAddr);

    while (1)
    {
        struct sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);

        SOCKET clientSocket = accept(serverSockets, (struct sockaddr*)&clientAddr, &clientAddrSize);

        if (clientSocket == INVALID_SOCKET)
        {
            ErrorHandling(L"accept() error!");
        }
        else
        {
            char clientIP[20] = { 0 };
            if (NULL == inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP))) // inet_ntop ip �ּҸ� ���ڿ� ���·� ��ȯ
            {
                ErrorHandling(L"inet_ntop() error!");
            }
            else
            {
                std::cout << "[Client IP: " << clientIP << "::" << ntohs(clientAddr.sin_port) << "]" << std::endl;
//                printf("[Client IP:%s, PORT: %d\n", clientIP, ntohs(clientAddr.sin_port)); // ntohs ��Ʈ�ѹ��� ���� ���·� ��ȯ
                clientSockets.push_back(clientSocket);

                std::thread clientThread(HandleClient, clientSocket);
                clientThread.detach();
            }
        }
    }

    // ����� Ŭ���̾�Ʈ ������ ����
    for (SOCKET clientSocket : clientSockets)
    {
        closesocket(clientSocket);
    }

    closesocket(serverSockets);

    WSACleanup();

    return 0;
}

void HandleClient(SOCKET clientSocket)
{
    char szBuffer[DEFAULT_BUFFER];
    int ret = 0;
    int sendret = 0;

    while (1)
    {
        ret = recv(clientSocket, szBuffer, DEFAULT_BUFFER, 0);
        if (ret == SOCKET_ERROR)
        {
            ErrorHandling(L"recv() error!");
            break;
        }
        else if (ret == 0)
        {
            std::cout << "Client disconnected.\n" << std::endl;
            break;
        }
        else
        {
            szBuffer[ret] = '\0';
            std::cout << "Received message from client:" << szBuffer << std::endl;

            sendret = send(clientSocket, szBuffer, ret, 0);
            if (sendret == SOCKET_ERROR)
            {
                ErrorHandling(L"send() error!");
                break;
            }
        }
    }

    closesocket(clientSocket);
}

void ErrorHandling(const std::wstring& message)
{
    std::wcout << message << std::endl;
    exit(1);
}