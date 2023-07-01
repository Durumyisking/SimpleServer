#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <thread>

#define DEFAULT_PORT   8080
#define MAX_BUFFER_SIZE 2048

#define ID_SIZE   20

struct Dataform
{
    std::string      name;
    std::string      message;
};


void Initialize();
void CreateSocket();
void SetServerDetails();
void BindServerSocket();
void ListenServerSocket();
void AcceptClient(sockaddr_in& _clientAddr, SOCKET& _clientSocket);
void JoinClient(sockaddr_in& _clientAddr, SOCKET& _clientSocket);
void ErrorHandling(const std::wstring& _message);
void HandleClient(SOCKET _clientSocket, Dataform _Data);
void PrintServerInfo(sockaddr_in _localAddr);
void PrintIPAddr();
void SendMessageToAllClient(const char* _Message, int _MessageLength);
bool ReceiveMessageFromClient(SOCKET _clientSocket, char* _Message, int _DataSize);

WSADATA WSdata;
WORD    Version;
SOCKET ServerSocket; // �������� : Ŭ���̾�Ʈ ���� ���� �� Ŭ���̾�Ʈ ���� ���� ���  
std::vector<SOCKET> ClientSockets; // Ŭ���̾�Ʈ ���ϵ��� ��� ����

sockaddr_in LocalAddr; // ���������� ���ε� �� �����ּ�

int main()
{
    Initialize(); // ���̺귯�� �ʱ�ȭ

    CreateSocket(); // ���� ���� ����

    SetServerDetails(); // ���� ��������, �������ּ� �� ��Ʈ��ȣ ����

    BindServerSocket(); // ���Ͽ� ���� �����ϵ� ���ε����� 

    ListenServerSocket(); // Ŭ���̾�Ʈ ���� �� �ִ� ���·� ���

    PrintServerInfo(LocalAddr);

    PrintIPAddr();

    std::cout << "Waiting Clients..." << std::endl;

    while (1)
    {
        // ������ ���µ��� ���ο� Ŭ���̾�Ʈ�� ��� �����ű� ������ while���ȿ��� Ŭ���̾�Ʈ ����
        sockaddr_in clientAddr;
        SOCKET clientSocket;
        AcceptClient(clientAddr, clientSocket);
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
        std::cout << "Winsock2 library initialized.\n";
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
        std::cout << "Server socket created.\n\n";
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
    // ���� ������ ����ϴ� ���� ��⿭ ����
    if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR) // SOMAXCONN (socket max connection) �ü������ �ٸ� �����쿡���� 1000 
    {
        ErrorHandling(L"listen() error!");
    }
}

void AcceptClient(sockaddr_in& _clientAddr, SOCKET& _clientSocket)
{
    int clientAddrSize = sizeof(_clientAddr);
    _clientSocket = accept(ServerSocket, (sockaddr*)&_clientAddr, &clientAddrSize);

    if (_clientSocket == INVALID_SOCKET)
    {
        closesocket(_clientSocket);
        closesocket(ServerSocket);
        ErrorHandling(L"accept() error!");
    }
    else
    {
        // Ŭ���̾�Ʈ �������Լ� ����
        JoinClient(_clientAddr, _clientSocket);
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

        Dataform ReceivedData = {};
        bool flag = ReceiveMessageFromClient(_clientSocket, reinterpret_cast<char*>(&ReceivedData), sizeof(Dataform));

        if (flag)
        {
            std::string str = {};
            str = ReceivedData.name;
            str += " ���� �����߽��ϴ�.";
            SendMessageToAllClient(str.c_str(), str.length());
                        
            std::thread clientThread(HandleClient, _clientSocket, ReceivedData);
            clientThread.detach();
        }
        ZeroMemory(&ReceivedData, sizeof(Dataform));
    }
}

void HandleClient(SOCKET _clientSocket, Dataform _Data)
{
//    char receivedMessageBuffer[MAX_BUFFER_SIZE];
    int receiveTest = 0;
    int sendTest = 0;

    while (1)
    {

        bool flag = ReceiveMessageFromClient(_clientSocket, reinterpret_cast<char*>(&_Data), sizeof(Dataform));
        
        if(flag)
        {
            std::cout << _Data.name << "���� �޽��� : " << _Data.message << std::endl;
            
            for (size_t i = 0; i < ClientSockets.size(); i++)
            {
                sendTest = send(ClientSockets[i], reinterpret_cast<const char*>(&_Data), receiveTest, 0);
            }
//            sendTest = send(_clientSocket, reinterpret_cast<const char*>(&ReceivedData), receiveTest, 0);
            ZeroMemory(&_Data, sizeof(Dataform));
            if (sendTest == SOCKET_ERROR)
            {
                ErrorHandling(L"send() error!");
                break;
            }
        }
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



///////////////////////////

void SendMessageToAllClient(const char* _Message, int _MessageLength)
{
    int sendTest = 0;

    for (size_t i = 0; i < ClientSockets.size(); i++)
    {
        sendTest = send(ClientSockets[i], _Message, _MessageLength, 0);
    }
    if (sendTest == SOCKET_ERROR)
    {
        ErrorHandling(L"send() error!");
    }
}

bool ReceiveMessageFromClient(SOCKET _clientSocket, char* _Message, int _DataSize)
{
    int receiveTest = 0;
    // recv�Լ��� ���� client�� send�� ���� �غ� �ϴ°�
    receiveTest = recv(_clientSocket, _Message, _DataSize, 0);

    if (receiveTest == SOCKET_ERROR)
    {
        ErrorHandling(L"recv() error!");
        return false;
    }
    else if (receiveTest == 0)
    {
        std::cout << "Client disconnected.\n" << std::endl;
        return false;
    }

    return true;
}