#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <thread>

#define DEFAULT_PORT   8080
#define MAX_BUFFER_SIZE 2048

void Initialize();
void CreateSocket();
void SetServerDetails();
void BindServerSocket();
void ListenServerSocket();
void AcceptClient(sockaddr_in& _clientAddr, SOCKET& _clientSocket);
void JoinClient(sockaddr_in& _clientAddr, SOCKET& _clientSocket);
void ErrorHandling(const std::wstring& _message);
void HandleClient(SOCKET _clientSocket);
void PrintServerInfo(sockaddr_in _localAddr);
void PrintIPAddr();

WSADATA WSdata;
WORD    Version;
SOCKET ServerSocket; // �������� : Ŭ���̾�Ʈ ���� ���� �� Ŭ���̾�Ʈ ���� ���� ���  
std::vector<SOCKET> ClientSockets; // Ŭ���̾�Ʈ ���ϵ��� ��� ����

sockaddr_in LocalAddr; // ���������� ���ε� �� �����ּ�

int main()
{
    Initialize();

    CreateSocket();

    SetServerDetails();

    BindServerSocket();

    ListenServerSocket();

    PrintServerInfo(LocalAddr);

    PrintIPAddr();

    std::cout << "Waiting Clients..." << std::endl;

    while (1)
    {
        // ������ ���µ��� ���ο� Ŭ���̾�Ʈ�� ��� �����ű� ������ while���ȿ��� Ŭ���̾�Ʈ ����
        sockaddr_in clientAddr;
        SOCKET clientSocket;
        AcceptClient(clientAddr, clientSocket);

        // Ŭ���̾�Ʈ �������Լ� ����
        JoinClient(clientAddr, clientSocket);        
    }

    // ����� Ŭ���̾�Ʈ ������ ����
    for (SOCKET clientSocket : ClientSockets)
    {
        closesocket(clientSocket);
    }

    closesocket(ServerSocket);

    WSACleanup();

    return 0;
}

void HandleClient(SOCKET _clientSocket)
{
    char RecievedMessageBuffer[MAX_BUFFER_SIZE];
    int recieveTest = 0;
    int sendTest = 0;

    while (1)
    {
        recieveTest = recv(_clientSocket, RecievedMessageBuffer, MAX_BUFFER_SIZE, 0);
        if (recieveTest == SOCKET_ERROR)
        {
            ErrorHandling(L"recv() error!");
            break;
        }
        else if (recieveTest == 0)
        {
            std::cout << "Client disconnected.\n" << std::endl;
            break;
        }
        else
        {
            ZeroMemory(RecievedMessageBuffer, MAX_BUFFER_SIZE);
            std::cout << "Received message from client:" << RecievedMessageBuffer << std::endl;

            sendTest = send(_clientSocket, RecievedMessageBuffer, recieveTest, 0);
            if (sendTest == SOCKET_ERROR)
            {
                ErrorHandling(L"send() error!");
                break;
            }
        }
    }

    closesocket(_clientSocket);
}

void Initialize()
{
    Version = MAKEWORD(2, 2);
    int wsResult = WSAStartup(Version, &WSdata);
    if (wsResult != 0)
    {
        ErrorHandling(L"WSAStartup() error!");
    }
    else
    {
        puts("The library has been initialized.\n\n");
    }
}

void CreateSocket()
{
    ServerSocket = socket(AF_INET, SOCK_STREAM, 0); // ���� ���� ����
    if (ServerSocket == INVALID_SOCKET)
    {
        ErrorHandling(L"socket() error!");
    }
    else
    {
        std::cout << "Server socket created.\n" << std::endl;
    }
}

void SetServerDetails()
{
    LocalAddr.sin_family = AF_INET; // IPv4�� �ּ� ü�� ����
    LocalAddr.sin_port = htons(DEFAULT_PORT); // ��Ʈ�ѹ� ����
    LocalAddr.sin_addr.S_un.S_addr = INADDR_ANY; // ������ �ּ� ���� (INADDR_ANY Ű����� ��� ��Ʈ��ũ �������̽����� ������ ������ ����)}
}

void BindServerSocket()
{
    if (bind(ServerSocket, (sockaddr*)&LocalAddr, sizeof(LocalAddr)) == SOCKET_ERROR)
    {
        ErrorHandling(L"bind() error!");
    }
}

void ListenServerSocket()
{
    if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        ErrorHandling(L"listen() error!");
    }
}

void AcceptClient(sockaddr_in& _clientAddr, SOCKET& _clientSocket)
{
    int clientAddrSize = sizeof(_clientAddr);
    SOCKET clientSocket = accept(ServerSocket, (sockaddr*)&_clientAddr, &clientAddrSize);

    if (clientSocket == INVALID_SOCKET)
    {
        closesocket(_clientSocket);
        closesocket(ServerSocket);
        ErrorHandling(L"accept() error!");
    }
}

void JoinClient(sockaddr_in& _clientAddr, SOCKET& _clientSocket)
{
    char clientIP[INET_ADDRSTRLEN];
    ZeroMemory(clientIP, INET_ADDRSTRLEN);
    if (NULL == inet_ntop(AF_INET, &_clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN)) // inet_ntop ip �ּҸ� ���ڿ� ���·� ��ȯ
    {
        ErrorHandling(L"inet_ntop() error!");
    }
    else
    {
        std::cout << "[Client IP: " << clientIP << "::" << ntohs(_clientAddr.sin_port) << "]" << std::endl;
        ClientSockets.push_back(_clientSocket);

        std::thread clientThread(HandleClient, _clientSocket);
        clientThread.detach();
    }
}

void ErrorHandling(const std::wstring& _message)
{
    std::wcout << _message << std::endl;
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