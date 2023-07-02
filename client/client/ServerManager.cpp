#include "ServerManager.h"
#include "UtilFunction.h"

WSADATA          ServerManager::mWSdata = {};
WORD             ServerManager::mWSVersion = {};
SOCKET           ServerManager::mSocket = {};
sockaddr_in      ServerManager::mServerAddr = {};

char             ServerManager::mTextRecieveBuffer[MAX_BUFFER_SIZE] = {};
std::string      ServerManager::mServerIP = {};
Dataform         ServerManager::mData = {};

int              ServerManager::mStartupTest = 0;
int              ServerManager::mConnectTest = 0;
int              ServerManager::mSendTest = 0;
int              ServerManager::mReceiveTest = 0;


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
        std::cout << "Winsock2 library initialized.\n";
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
        std::cout << "Client socket created.\n";
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
    std::cout << "닉네임을 입력하세요 : ";
    gets_s(mData.name);
    while (ID_SIZE < sizeof(mData.name))
    {       
        ZeroMemory(mData.name, ID_SIZE);
        std::cout << "20자 이내의 닉네임을 입력하셔야합니다. 다시 입력해주세요." << std::endl;
        std::cout << "닉네임을 입력하세요 : ";
    }

    mConnectTest = connect(mSocket, reinterpret_cast<sockaddr*>(&mServerAddr), sizeof(mServerAddr));
    if (mConnectTest == SOCKET_ERROR)
    {
        closesocket(mSocket);
        ErrorHandling(L"connect() error!");
    }
    else
    {
        std::cout << "Connected to the server.\n\n";
    }

    sendData();
    receiveMessage();
}

void ServerManager::chatSend()
{
    std::cout << "Send Message: "; 
    gets_s(mData.message);

    const char* Buffer = reinterpret_cast<const char*>(&mData);

    mSendTest = send(mSocket, Buffer, DATA_SIZE, 0);
    ZeroMemory(mData.message, MSG_SIZE);

    if (mSendTest == SOCKET_ERROR)
    {
        ErrorHandling(L"send() error!");
    }
    else
    {
        UtilFunction::ClearConsoleLine();
    }
}

void ServerManager::chatReceive()
{
    ZeroMemory(mTextRecieveBuffer, MAX_BUFFER_SIZE);
    mReceiveTest = recv(mSocket, mTextRecieveBuffer, MAX_BUFFER_SIZE, 0);
    if (mReceiveTest == SOCKET_ERROR)
    {
        ErrorHandling(L"recv() error!");
    }
    else if (mReceiveTest == 0)
    {
        ErrorHandling(L"Server disconnected.");
    }
    else
    {
        Dataform receivedData = {};
        memcpy(&receivedData, mTextRecieveBuffer, DATA_SIZE);
        std::cout << receivedData.name << "님의 메시지: " << receivedData.message << std::endl;
        ZeroMemory(&receivedData, DATA_SIZE);

    }
}

void ServerManager::receiveMessage()
{
    mReceiveTest = 0;
    // recv함수에 들어가면 client의 send를 받을 준비를 하는것
    mReceiveTest = recv(mSocket, mTextRecieveBuffer, MAX_BUFFER_SIZE, 0);

    if (mReceiveTest == SOCKET_ERROR)
    {
        ErrorHandling(L"recv() error!");
        return;
    }
    else if (mReceiveTest == 0)
    {
        std::cout << "Client disconnected.\n" << std::endl;
        return;
    }
    else
    {
        std::cout << mTextRecieveBuffer << std::endl;
    }
}

void ServerManager::receiveData()
{
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

void ServerManager::sendMessage(std::string _Message)
{
}

void ServerManager::sendData()
{
    const char* Buffer = reinterpret_cast<const char*>(&mData);

    mSendTest = send(mSocket, Buffer, DATA_SIZE, 0);
    if (mSendTest == SOCKET_ERROR)
    {
        ErrorHandling(L"send() error!");
    }
    else
    {
        UtilFunction::ClearConsoleLine();
    }
}
