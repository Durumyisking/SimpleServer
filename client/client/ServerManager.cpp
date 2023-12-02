#include "ServerManager.h"
#include "UtilFunction.h"

WSADATA          ServerManager::mWSdata = {};
WORD             ServerManager::mWSVersion = {};
SOCKET           ServerManager::mSocket = {};
sockaddr_in      ServerManager::mServerAddr = {};

char             ServerManager::mRecieveBuffer[MAX_BUFFER_SIZE] = {};
char             ServerManager::mServerIP[MAX_BUFFER_SIZE] = {};
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
    gets_s(mServerIP);

    // 127.0.0.1 = ���� ȣ��Ʈ �ּ� (���� ��ǻ�� ��ü�� ����Ŵ ������ Ŭ���̾�Ʈ�� ������ pc���� ���� �� ��� ���Ҷ� ���)
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
    inet_pton(AF_INET, mServerIP, &(mServerAddr.sin_addr));
}

void ServerManager::connectToServer()
{
    std::cout << "�г����� �Է��ϼ��� : ";
    gets_s(mData.name);
    while (ID_SIZE < sizeof(mData.name))
    {       
        ZeroMemory(mData.name, ID_SIZE);
        std::cout << "20�� �̳��� �г����� �Է��ϼž��մϴ�. �ٽ� �Է����ּ���." << std::endl;
        std::cout << "�г����� �Է��ϼ��� : ";
    }

    makeConnection(mSocket, mServerAddr);
    std::cout << "Connected to the server.\n\n";

    // �ڱ��ڽ� ����˸��°�
    sendMessage(ePacketType::UserJoin);
    receiveMessage();

    std::cout << "����Ǿ����ϴ�! ���� �����Ӱ� �޽����� �Է��ϼ���" << std::endl;

}


void ServerManager::sendMessage(ePacketType packetType)
{
    while (mbWhileflag)
    {
        mData.PacketType = packetType;

        switch (mData.PacketType)
        {
        case ePacketType::UserJoin:
            break;
        case ePacketType::Message:
            gets_s(mData.message);
            break;
        default:
            break;
        }

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
}

void ServerManager::receiveMessage()
{
    while (mbWhileflag)
    {
        int ReceiveTest = 0;
        // recv�Լ��� ���� client�� send�� ���� �غ� �ϴ°�
        ZeroMemory(mRecieveBuffer, MAX_BUFFER_SIZE);
        ReceiveTest = recv(mSocket, mRecieveBuffer, MAX_BUFFER_SIZE, 0);

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
            Dataform receivedData = {};
            memcpy(&receivedData, mRecieveBuffer, DATA_SIZE);

            switch (receivedData.PacketType)
            {
            case ePacketType::UserJoin:
                std::cout << receivedData.name << "���� �����ϼ̽��ϴ�." << std::endl;
                break;
            case ePacketType::Message:
                std::cout << receivedData.name << "���� �޽��� : " 
                    << receivedData.message << std::endl;
                break;
            default:
                break;
            }

        }
    }
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

