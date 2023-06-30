#include "ServerManager.h"
#include "UtilFunction.h"

WSADATA          ServerManager::mWSdata = {};
WORD             ServerManager::mWSVersion = {};
SOCKET           ServerManager::mSocket = {};
sockaddr_in      ServerManager::mServerAddr = {};

char             ServerManager::mTextRecieveBuffer[MAX_BUFFER_SIZE] = {};
std::string      ServerManager::mServerIP = {};
SendData         ServerManager::mData = {};

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
        std::cout << "Winsock2 library initialized.\n";
    }
}

void ServerManager::setServerIP()
{
    //std::cout << "Input Connect Server IP: ";
    //std::cin >> serverIP;
    // 127.0.0.1 = ���� ȣ��Ʈ �ּ� (���� ��ǻ�� ��ü�� ����Ŵ ������ Ŭ���̾�Ʈ�� ������ pc���� ���ؿ��� ��� ���Ҷ� ���)
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

    std::cout << "�г����� �Է��ϼ��� : ";
    std::cin >> mData.name;

    while (ID_SIZE < mData.name.size())
    {
        std::cout << "20�� �̳��� �г����� �Է��ϼž��մϴ�. �ٽ� �Է����ּ���.";
        std::cout << "�г����� �Է��ϼ��� : ";
        std::cin >> mData.name;
    }
}

void ServerManager::sendMessage()
{
    std::cout << "Send Message: ";
    std::cin >> mData.message;

    const char* Buffer = reinterpret_cast<const char*>(&mData);

    mSendTest = send(mSocket, Buffer, sizeof(SendData), 0);
    if (mSendTest == SOCKET_ERROR)
    {
        ErrorHandling(L"send() error!");
    }
    else
    {
        UtilFunction::ClearConsoleLine();
    }
}

void ServerManager::recieveMessage()
{
    ZeroMemory(mTextRecieveBuffer, sizeof(SendData));
    mRecieveTest = recv(mSocket, mTextRecieveBuffer, sizeof(SendData), 0);
    if (mRecieveTest == SOCKET_ERROR)
    {
        ErrorHandling(L"recv() error!");
    }
    else if (mRecieveTest == 0)
    {
        ErrorHandling(L"Server disconnected.");
    }
    else
    {
        SendData* receivedData =  new SendData();
        memcpy(receivedData, mTextRecieveBuffer, sizeof(SendData));
    
        std::cout << receivedData->name << "���� �޽���: " << receivedData->message << std::endl;

        delete receivedData;
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