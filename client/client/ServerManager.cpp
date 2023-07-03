#include "ServerManager.h"
#include "UtilFunction.h"

WSADATA          ServerManager::mWSdata = {};
WORD             ServerManager::mWSVersion = {};
SOCKET           ServerManager::mJoinSocket = {};
SOCKET           ServerManager::mSocket = {};
sockaddr_in      ServerManager::mServerAddr = {};

char             ServerManager::mTextRecieveBuffer[MAX_BUFFER_SIZE] = {};
std::string      ServerManager::mServerIP = {};
Dataform         ServerManager::mData = {};

int              ServerManager::mStartupTest = 0;
int              ServerManager::mConnectTest = 0;
int              ServerManager::mSendTest = 0;

bool ServerManager::mbWhileflag = true;
bool ServerManager::mbSwitch = true;



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
    std::cout << "Input Connect Server IP: ";
    std::cin >> mServerIP;
    // 127.0.0.1 = 로컬 호스트 주소 (현재 컴퓨터 자체를 가리킴 서버와 클라이언트가 동일한 pc에서 실행 및 통신 원할때 사용)
//    mServerIP = LOCALHOST_IP;
}

void ServerManager::createSocket(SOCKET& _Socket)
{
    _Socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_Socket == INVALID_SOCKET)
    {
        ErrorHandling(L"socket() error");
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

    makeConnection(mJoinSocket, mServerAddr);
    makeConnection(mSocket, mServerAddr);
    std::cout << "Connected to the server.\n\n";

    sendData();

    // 자기자신 입장알리는것
    receiveMessage(mJoinSocket);

    std::cout << "연결되었습니다! 이제 자유롭게 메시지를 입력하세요" << std::endl;

}

void ServerManager::joinReceive()
{
    while (1)
    {
        int ReceiveTest = 0;
        ZeroMemory(mTextRecieveBuffer, MAX_BUFFER_SIZE);
        ReceiveTest = recv(mJoinSocket, mTextRecieveBuffer, MAX_BUFFER_SIZE, 0);
        if (ReceiveTest == SOCKET_ERROR)
        {
            ErrorHandling(L"recv() error!");
        }
        else if (ReceiveTest == 0)
        {
            ErrorHandling(L"Server disconnected.");
        }
        else
        {
            std::cout << mTextRecieveBuffer << std::endl;
        }
    }
}

void ServerManager::chatSend()
{
    while (1)
    {
//        std::cout << "Send Message: ";
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
}

void ServerManager::chatReceive()
{
    while (1)
    {
        int ReceiveTest = 0;
        ZeroMemory(mTextRecieveBuffer, MAX_BUFFER_SIZE);
        ReceiveTest = recv(mSocket, mTextRecieveBuffer, MAX_BUFFER_SIZE, 0);
        if (ReceiveTest == SOCKET_ERROR)
        {
            ErrorHandling(L"recv() error!");
        }
        else if (ReceiveTest == 0)
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
}

void ServerManager::receiveMessage(SOCKET _Socket)
{
    int ReceiveTest = 0;
    // recv함수에 들어가면 client의 send를 받을 준비를 하는것
    ReceiveTest = recv(_Socket, mTextRecieveBuffer, MAX_BUFFER_SIZE, 0);

    if (ReceiveTest == SOCKET_ERROR)
    {
        ErrorHandling(L"recv() error!");
        return;
    }
    else if (ReceiveTest == 0)
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

void ServerManager::makeConnection(SOCKET _Socket, sockaddr_in _ServerAddr)
{
    mConnectTest = connect(_Socket, reinterpret_cast<sockaddr*>(&_ServerAddr), sizeof(_ServerAddr));
    if (mConnectTest == SOCKET_ERROR)
    {
        closesocket(_Socket);
        ErrorHandling(L"connect() error!");
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
    disConnect();
    mbSwitch = false;

//    exit(1);
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
