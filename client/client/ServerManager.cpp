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

int ServerManager::mbIsConnected = 0;
bool ServerManager::mbIsNicknameSet = false;
bool ServerManager::mbIsMsgSent = false;
std::vector<Dataform>      ServerManager::mDataTable = {};

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
    if (mbIsConnected == -1)
    {
        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f,1.f), u8"올바르지 않은 IP입니다 다시 입력해주세요");
    }

    ImGui::AlignTextToFramePadding();
    ImGui::InputTextWithHint(u8"##InputTextWithHint1", u8"서버 IP를 입력하세요 (ex-> xxx.xxx.xxx...)", ServerManager::mServerIP, sizeof(ServerManager::mServerIP));
    ImGui::SameLine();
    // 127.0.0.1 = 로컬 호스트 주소 (현재 컴퓨터 자체를 가리킴 서버와 클라이언트가 동일한 pc에서 실행 및 통신 원할때 사용)
    //  mServerIP = LOCALHOST_IP;

    // ip 컨버트

    if (ImGui::Button(u8"확인") || (ImGui::IsKeyPressed(ImGuiKey_Enter)))
    {
        ServerManager::convertIP();
        ServerManager::connectToServer();
    }
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
    inet_pton(AF_INET, "127.0.0.1", &(mServerAddr.sin_addr));
}

void ServerManager::connectToServer()
{
    if (!mSocket)
    {
        createSocket(mSocket);
    }

    if (!makeConnection(mSocket, mServerAddr))
    {
        return;
    }
    std::cout << "Connected to the server.\n\n";
}


void ServerManager::sendMessage(ePacketType packetType, bool bOnce)
{
    while (mbWhileflag)
    {
        if (mbIsMsgSent)
        {
            mData.PacketType = packetType;

            switch (mData.PacketType)
            {
            case ePacketType::UserJoin:
                break;
            case ePacketType::Message:
                    
                if ('\0' == mData.message[0])
                {
                    return;
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
            ZeroMemory(&mData.message, MSG_SIZE);
            mbIsMsgSent = false;
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
        // recv함수에 들어가면 client의 send를 받을 준비를 하는것
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
            mDataTable.push_back(*receivedData);
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
        ServerManager::mbIsConnected = -1;
        return false;
    }
    else
    {
        ServerManager::mbIsConnected = 1;
        return true;
    }

    return false;
}

void ServerManager::participateUserThreads()
{
    // 메시지 송수신
    // 플레이어 입장 채팅 송수신 전부 다른쓰레드에서 동작해야함
    std::thread sendThread(ServerManager::sendMessage, ePacketType::Message, false);
    sendThread.detach();

    std::thread receiveThread(ServerManager::receiveMessage, false);
    receiveThread.detach();

}

void ServerManager::processCurrentUserJoin()
{
    // 자기자신 입장알리는것
    mbIsMsgSent = true;
    sendMessage(ePacketType::UserJoin, true);
    receiveMessage(true);

    std::cout << "연결되었습니다! 이제 자유롭게 메시지를 입력하세요" << std::endl;
    participateUserThreads();
}

void ServerManager::setUserNickName()
{
    ImGui::InputTextWithHint("##InputTextWithHint2", u8"닉네임을 정해주세요", mData.name, sizeof(mData.name));
    ImGui::SameLine();
    if (ImGui::Button(u8"결정") || (ImGui::IsKeyPressed(ImGuiKey_Enter)))
    {     
        mbIsNicknameSet = true;      

        processCurrentUserJoin();
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

