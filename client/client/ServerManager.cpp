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
bool ServerManager::mbIsConnected = false;



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
    ImGui::InputTextWithHint("", "���� IP�� �Է��ϼ��� (ex-> xxx.xxx.xxx...)", ServerManager::mServerIP, sizeof(ServerManager::mServerIP));
    // 127.0.0.1 = ���� ȣ��Ʈ �ּ� (���� ��ǻ�� ��ü�� ����Ŵ ������ Ŭ���̾�Ʈ�� ������ pc���� ���� �� ��� ���Ҷ� ���)
    //  mServerIP = LOCALHOST_IP;

        // ip ����Ʈ
    ServerManager::convertIP();
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
    if (!mSocket)
    {
        createSocket(mSocket);
    }

    std::cout << "Sockets are created.\n";

    if (makeConnection(mSocket, mServerAddr))
    {
        return;
    }
    std::cout << "Connected to the server.\n\n";

    setUserNickName();
}


void ServerManager::sendMessage(ePacketType packetType, bool bOnce)
{
    while (mbWhileflag)
    {
        mData.PacketType = packetType;

        switch (mData.PacketType)
        {
        case ePacketType::UserJoin:
            break;
        case ePacketType::Message:
            while (1)
            {
                gets_s(mData.message);
                
                if ('\0' == mData.message[0])
                {
                    int i = 0;
                }
                else
                {
                    break;
                }
            }
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

        if (bOnce)
            break;
    }
}

void ServerManager::receiveMessage(bool bOnce)
{
    while (mbWhileflag)
    {
        int ReceiveTest = 0;
        // recv�Լ��� ���� client�� send�� ���� �غ� �ϴ°�
        ZeroMemory(mRecieveBuffer, MAX_BUFFER_SIZE);
        ReceiveTest = recv(mSocket, mRecieveBuffer, DATA_SIZE, 0);

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
            Dataform* receivedData = reinterpret_cast<Dataform*>(&mRecieveBuffer);

            std::cout << " ";
            switch (receivedData->PacketType)
            {
            case ePacketType::UserJoin:
                std::cout << receivedData->name << "���� �����ϼ̽��ϴ�." << std::endl;
                break;
            case ePacketType::Message:
                std::cout << receivedData->name << "���� �޽��� : "
                    << receivedData->message << std::endl;
                break;
            default:
                break;
            }

        }

        if (bOnce)
            break;
    }
}



bool ServerManager::makeConnection(SOCKET _Socket, sockaddr_in _ServerAddr)
{
    mConnectTest = connect(_Socket, reinterpret_cast<sockaddr*>(&_ServerAddr), sizeof(_ServerAddr));
    if (mConnectTest == SOCKET_ERROR)
    {
        //closesocket(_Socket);
        //ErrorHandling(L"connect() error!");
        ZeroMemory(mServerIP, MAX_BUFFER_SIZE);
        return false;
    }
    else
    {
        ServerManager::mbIsConnected = true;
        return true;
    }

    return false;
}

void ServerManager::participateUserThreads()
{
    // �޽��� �ۼ���
    // �÷��̾� ���� ä�� �ۼ��� ���� �ٸ������忡�� �����ؾ���
    std::thread sendThread(ServerManager::sendMessage, ePacketType::Message, false);
    sendThread.detach();

    std::thread receiveThread(ServerManager::receiveMessage, false);
    receiveThread.detach();

}

void ServerManager::processCurrentUserJoin()
{
    // �ڱ��ڽ� ����˸��°�
    sendMessage(ePacketType::UserJoin, true);
    receiveMessage(true);

    std::cout << "����Ǿ����ϴ�! ���� �����Ӱ� �޽����� �Է��ϼ���" << std::endl;
    participateUserThreads();
}

void ServerManager::setUserNickName()
{
    ImGui::InputTextWithHint("", "�г����� �����ּ���", mData.name, sizeof(mData.name));
    ImGui::SameLine();
    if (ImGui::Button("����"))
    {
        if (ID_SIZE < sizeof(mData.name))
        {
            ZeroMemory(mData.name, ID_SIZE);
            std::cout << "20�� �̳��� �г����� �Է��ϼž��մϴ�. �ٽ� �Է����ּ���." << std::endl;
            std::cout << "�г����� �Է��ϼ��� : ";
        }
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

