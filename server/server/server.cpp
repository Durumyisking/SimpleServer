#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <thread>

#define DEFAULT_PORT   8080
#define MAX_BUFFER_SIZE 2048

void ErrorHandling(const std::wstring& message);
void HandleClient(SOCKET clientSocket);
void PrintServerInfo(sockaddr_in _localAddr);
void PrintIPAddr();

int main()
{
    WSADATA wsd;
    WORD version;
    SOCKET serverSockets; // �������� : Ŭ���̾�Ʈ ���� ���� �� Ŭ���̾�Ʈ ���� ���� ���  
    std::vector<SOCKET> clientSockets; // Ŭ���̾�Ʈ ���ϵ��� ��� ����

    sockaddr_in localAddr; // ���������� ���ε� �� �����ּ�


    // �̴ϼȶ�����
    version = MAKEWORD(2, 2);
    int wsResult = WSAStartup(version, &wsd);
    if (wsResult != 0)
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

    localAddr.sin_family = AF_INET; // IPv4�� �ּ� ü�� ����
    localAddr.sin_port = htons(DEFAULT_PORT); // ��Ʈ�ѹ� ����
    localAddr.sin_addr.S_un.S_addr = INADDR_ANY; // ������ �ּ� ���� (INADDR_ANY Ű����� ��� ��Ʈ��ũ �������̽����� ������ ������ ����)

    if (bind(serverSockets, (sockaddr*)&localAddr, sizeof(localAddr)) == SOCKET_ERROR)
    {
        ErrorHandling(L"bind() error!");
    }

    if (listen(serverSockets, SOMAXCONN) == SOCKET_ERROR)
    {
        ErrorHandling(L"listen() error!");
    }

    PrintServerInfo(localAddr);
    PrintIPAddr();

    std::cout << "Waiting Clients..." << std::endl;

    while (1)
    {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSockets, (sockaddr*)&clientAddr, &clientAddrSize);

        if (clientSocket == INVALID_SOCKET)
        {
            closesocket(serverSockets);
            ErrorHandling(L"accept() error!");
        }
        else
        {
            char clientIP[INET_ADDRSTRLEN];
            ZeroMemory(clientIP, INET_ADDRSTRLEN);
            if (NULL == inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN)) // inet_ntop ip �ּҸ� ���ڿ� ���·� ��ȯ
            {
                ErrorHandling(L"inet_ntop() error!");
            }
            else
            {
                std::cout << "[Client IP: " << clientIP << "::" << ntohs(clientAddr.sin_port) << "]" << std::endl;
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
    char szBuffer[MAX_BUFFER_SIZE];
    int ret = 0;
    int sendret = 0;

    while (1)
    {
        ret = recv(clientSocket, szBuffer, MAX_BUFFER_SIZE, 0);
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
            ZeroMemory(szBuffer, MAX_BUFFER_SIZE);
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
    WSACleanup();
    exit(1);
}

void PrintServerInfo(sockaddr_in _localAddr)
{
    char serverIP[20] = { 0 };
    if (NULL == inet_ntop(AF_INET, &_localAddr.sin_addr, serverIP, sizeof(serverIP))) // inet_ntop ip �ּҸ� ���ڿ� ���·� ��ȯ
    {
        ErrorHandling(L"inet_ntop() error!");
    }
    else
    {
        std::cout << "[Server IP: " << serverIP << "::" << ntohs(_localAddr.sin_port) << "]" << std::endl;
    }

}


void PrintIPAddr()
{
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        std::cout << "Failed to get hostname" << std::endl;
        WSACleanup();
        return;
    }

    struct addrinfo* ipAddr = nullptr;
    struct addrinfo hints {};
    hints.ai_family = AF_INET;

    if (getaddrinfo(hostname, nullptr, &hints, &ipAddr) != 0) {
        std::cout << "Failed to get IP address" << std::endl;
        WSACleanup();
        return;
    }

    struct sockaddr_in* addr = reinterpret_cast<struct sockaddr_in*>(ipAddr->ai_addr);
    char ipAddress[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &(addr->sin_addr), ipAddress, INET_ADDRSTRLEN) == nullptr) {
        std::cout << "Failed to convert IP address to string" << std::endl;
        WSACleanup();
        return;
    }

    std::cout << "IP address: " << ipAddress << std::endl;

    freeaddrinfo(ipAddr);
}