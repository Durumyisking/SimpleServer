#include "ServerManager.h"

WSADATA          ServerManager::mWSdata = {};
WORD             ServerManager::mWSVersion = {};
SOCKET           ServerManager::mSocket = {};
sockaddr_in      ServerManager::mServerAddr = {};

char             ServerManager::mTextRecieveBuffer[MAX_BUFFER_SIZE] = {};
std::string      ServerManager::mServerIP ={};
std::string      ServerManager::mMessage = {};

int              ServerManager::mStartupTest = 0;
int              ServerManager::mConnectTest = 0;
int              ServerManager::mSendTest = 0;
int              ServerManager::mRecieveTest = 0;


void ServerManager::initialize()
{
    mWSVersion = MAKEWORD(2, 2);
    int mStartupTest = WSAStartup(mWSVersion, &mWSdata);
    if (mStartupTest != 0)
    {
        ErrorHandling(L"WSAStartup() error!");
    }
    else
    {
        std::cout << "The library has been initialized.\n";
    }
}

void ServerManager::setServerIP()
{
    //std::cout << "Input Connect Server IP: ";
    //std::cin >> serverIP;
    // 127.0.0.1 = 로컬 호스트 주소 (현재 컴퓨터 자체를 가리킴 서버와 클라이언트가 동일한 pc에서 실해오딕 통신 원할때 사용)
    mServerIP = LOCALHOST_IP;
}

void ServerManager::createSocket()
{
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket == INVALID_SOCKET)
    {
        ErrorHandling(L"socket() error");
    }
    else
    {
        std::cout << "Socket has been created.\n";
    }
}

void ServerManager::convertIP()
{
    mServerAddr.sin_family = AF_INET;
    mServerAddr.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, mServerIP.c_str(), &(mServerAddr.sin_addr));
}

void ServerManager::connectToServer()
{
    mConnectTest = connect(mSocket, reinterpret_cast<sockaddr*>(&mServerAddr), sizeof(mServerAddr));
    if (mConnectTest == SOCKET_ERROR)
    {
        closesocket(mSocket);
        ErrorHandling(L"connect() error!");
    }
    else
    {
        std::cout << "Connected to the server.\n";
    }
}

void ServerManager::sendMessage()
{
    std::cout << "Send Message: ";
    std::cin >> mMessage;

    mSendTest = send(mSocket, mMessage.c_str(), mMessage.size() + 1, 0);
    if (mSendTest == SOCKET_ERROR)
    {
        ErrorHandling(L"send() error!");
    }
}

void ServerManager::recieveMessage()
{
    ZeroMemory(mTextRecieveBuffer, MAX_BUFFER_SIZE);
    mRecieveTest = recv(mSocket, mTextRecieveBuffer, MAX_BUFFER_SIZE, 0);
    if (mRecieveTest == SOCKET_ERROR)
    {
        ErrorHandling(L"recv() error!");
    }
    else
    {
        std::cout << "Received Message : " << mTextRecieveBuffer << std::endl;;
    }
}

void ServerManager::disConnect()
{
    WSACleanup();
    closesocket(mSocket);
}

void ServerManager::ErrorHandling(const std::wstring& message)
{
    std::wcout << message << std::endl;
    WSACleanup();
    exit(1);
}
