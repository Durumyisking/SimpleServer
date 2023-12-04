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
#define MSG_SIZE   255
#define DATA_SIZE  275

enum class ePacketType
{
    UserJoin,
    Message,

    End,
};


struct Dataform
{
    ePacketType     PacketType;
    char            name[ID_SIZE];
    char            message[MSG_SIZE];
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
void SendMessageToAllClient(const Dataform& data);
void ReceiveMessageFromClient();
void PrintServerInfo(sockaddr_in _localAddr);
void PrintIPAddr();
void SendMessageToAllClient(std::vector<SOCKET> _Sockets, const char* _Message, int _MessageLength);

void CloseClientSocket(SOCKET _clientSocket);
void CloseServer();

bool ReceiveMessageFromClient(SOCKET _clientSocket, char* _Message, int _DataSize);

WSADATA WSdata;
WORD    Version;
SOCKET ServerSocket; // 서버소켓 : 클라이언트 연결 수락 및 클라이언트 소켓 생성 담당  
std::vector<SOCKET> ClientSockets; // 클라이언트 소켓들을 담는 벡터

sockaddr_in LocalAddr; // 서버소켓이 바인딩 될 로컬주소

int SendTest;
int ReceiveTest;

int main()
{
    Initialize(); // 라이브러리 초기화

    CreateSocket(); // 서버 소켓 생성

    SetServerDetails(); // 서버 프토토콜, 아이피주소 및 포트번호 설정

    BindServerSocket(); // 소켓에 서버 디테일들 바인딩해줌 

    ListenServerSocket(); // 클라이언트 받을 수 있는 상태로 대기

    PrintServerInfo(LocalAddr);

    PrintIPAddr();

    std::cout << "Waiting Clients..." << std::endl;

    while (1)
    {
        // 서버가 도는동안 새로운 클라이언트를 계속 받을거기 때문에 while문안에서 클라이언트 받음
        sockaddr_in clientAddr;

        SOCKET clientSocket;
        AcceptClient(clientAddr, clientSocket);
    }   

    // 연결된 클라이언트 소켓을 닫음
    CloseServer();

    return 0;
}


void Initialize()
{
    Version = MAKEWORD(2, 2);
    size_t s = sizeof(Version);
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
{                   // ip v4형식      SOCK_STREAM : AF_INET v4,v6에서 TCP를 사용
                    //               SOCK_DGRAM : UDP에서 사용
    ServerSocket = socket(AF_INET, SOCK_STREAM, 0); // 서버 소켓 생성
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
    LocalAddr.sin_family = AF_INET; // IPv4로 주소 체계 설정
    LocalAddr.sin_port = htons(DEFAULT_PORT); // 포트넘버 설정
    LocalAddr.sin_addr.S_un.S_addr = INADDR_ANY; // 아이피 주소 설정 (INADDR_ANY 키워드로 모든 네트워크 인터페이스에서 들어오는 연결을 수락)}
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
    // 서버 소켓이 대기하는 연결 대기열 생성
    if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR) // SOMAXCONN (socket max connection) 운영체제마다 다름 윈도우에서는 1000 
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
        ErrorHandling(L"accept() error!");
    }
    else
    {
        // 클라이언트 쓰레드함수 실행
        JoinClient(_clientAddr, _clientSocket);
    }
}

void JoinClient(sockaddr_in& _clientAddr, SOCKET& _clientSocket)
{
    char clientIP[INET_ADDRSTRLEN];
    ZeroMemory(clientIP, INET_ADDRSTRLEN);
    if (NULL == inet_ntop(AF_INET, &_clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN)) // inet_ntop ip 주소를 문자열 형태로 변환
    {
        ErrorHandling(L"inet_ntop() error!");
    }
    else
    {
        std::cout << "[Client IP: " << clientIP << "::" << ntohs(_clientAddr.sin_port) << "]" << std::endl;
        
        ClientSockets.push_back(_clientSocket);

        Dataform ReceivedData = {};
        ReceiveTest = ReceiveMessageFromClient(_clientSocket, reinterpret_cast<char*>(&ReceivedData), DATA_SIZE);

        if (ReceiveTest)
        {
            // 클라이언트 입장알림
            ReceivedData.PacketType = ePacketType::UserJoin;
            SendMessageToAllClient(ReceivedData);
                        
            // 새로 들어온 클라이언트 새 쓰레드에 올림
            std::thread clientThread(HandleClient, _clientSocket, ReceivedData);
            clientThread.detach();
        }
        ZeroMemory(&ReceivedData, DATA_SIZE);
    }
}

void HandleClient(SOCKET _clientSocket, Dataform _Data)
{
//    char receivedMessageBuffer[MAX_BUFFER_SIZE];
    ReceiveTest = 0;
    SendTest = 0;

    while (1)
    {
        ReceiveTest = ReceiveMessageFromClient(_clientSocket, reinterpret_cast<char*>(&_Data), DATA_SIZE);
        
        // 정상적으로 받았으면 해당 메시지 모든 클라에게 보낸다.
        if (ReceiveTest)
        {
            SendMessageToAllClient(_Data);
        }
        else
        {
            break;
        }
    }
}
void SendMessageToAllClient(const Dataform& data)
{
    SendTest = 0;

    const char* Buffer = reinterpret_cast<const char*>(&data);
    for (size_t i = 0; i < ClientSockets.size(); i++)
    {
        SendTest = send(ClientSockets[i], Buffer, DATA_SIZE, 0);
        if (SendTest == SOCKET_ERROR)
        {
            ErrorHandling(L"send() error!");
            break;
        }
    }
}
void ReceiveMessageFromClient()
{
}
void ErrorHandling(const std::wstring& _message)
{
    std::wcout << _message << std::endl;
    CloseServer();
    exit(1);
}

void PrintServerInfo(sockaddr_in _localAddr)
{
    char serverIP[20] = { 0 };
    if (NULL == inet_ntop(AF_INET, &_localAddr.sin_addr, serverIP, sizeof(serverIP))) // inet_ntop ip 주소를 문자열 형태로 변환
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
void SendMessageToAllClient(std::vector<SOCKET> _Sockets, const char* _Message, int _MessageLength)
{
    SendTest = 0;

    for (size_t i = 0; i < _Sockets.size(); i++)
    {
        SendTest = send(_Sockets[i], _Message, _MessageLength, 0);
        if (SendTest == SOCKET_ERROR)
        {
            ErrorHandling(L"send() error!");
        }
    }
}

void CloseClientSocket(SOCKET _clientSocket)
{
    for (size_t i = 0; i < ClientSockets.size(); i++)
    {
        if (ClientSockets[i] == _clientSocket)
        {
            ClientSockets.erase(ClientSockets.begin() + i);
            closesocket(_clientSocket);
            return;
        }
    }
}

void CloseServer()
{
    for (SOCKET clientSocket : ClientSockets)
    {
        closesocket(clientSocket);
    }

    closesocket(ServerSocket);

    WSACleanup();
}

void SendMessageToAllClientExceptSelf(SOCKET _clientSocket, const char* _Message, int _MessageLength)
{
    SendTest = 0;

    for (size_t i = 0; i < ClientSockets.size(); i++)
    {
        if (_clientSocket != ClientSockets[i])
        {
            SendTest = send(ClientSockets[i], _Message, _MessageLength, 0);
            if (SendTest == SOCKET_ERROR)
            {
                ErrorHandling(L"send() error!");
            }
        }
    }
}

bool ReceiveMessageFromClient(SOCKET _clientSocket, char* _Message, int _DataSize)
{
    ReceiveTest = 0;
    // recv함수에 들어가면 client의 send를 받을 준비를 하는것
    ReceiveTest = recv(_clientSocket, _Message, _DataSize, 0);

    if (ReceiveTest == SOCKET_ERROR)
    {
        std::cout << "recv() error!.\n" << std::endl;
        CloseClientSocket(_clientSocket);
        return false;
    }
    else if (ReceiveTest == 0)
    {
        std::cout << "Client disconnected.\n" << std::endl;
        CloseClientSocket(_clientSocket);
        return false;
    }

    
    return true;
}