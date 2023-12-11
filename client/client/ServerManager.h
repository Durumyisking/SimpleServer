#pragma once
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <minwinbase.h> // zeroMemory ��� ����
#include <string>
#include <stdio.h>
#include <thread>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <memory>
#include <wrl.h> // ComPtr
#include <vector>

#define DEFAULT_PORT   8080
#define MAX_BUFFER_SIZE 2048

#define IP_SIZE 20
#define LOCALHOST_IP "127.0.0.1" // ���� ȣ��Ʈ �ּ� (���� ��ǻ�� ��ü�� ����Ŵ ������ Ŭ���̾�Ʈ�� ������ pc���� ���ؿ��� ��� ���Ҷ� ���)
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

class ServerManager
{
public:
    static void initialize();
    static void setServerIP();
    static void createSocket(SOCKET& _Socket);
    static void convertIP();
    static void connectToServer();

    static void disConnect();

    static void ErrorHandling(const std::wstring& message);
    static void sendMessage(ePacketType packetType, bool bOnce = false);
    static void receiveMessage(bool bOnce = false);

    static void setUserNickName();
private:
    static bool makeConnection(SOCKET _Socket, sockaddr_in _ServerAddr);
    static void participateUserThreads();
    static void processCurrentUserJoin();

public:
    static WSADATA          mWSdata;
    static WORD             mWSVersion;
    static SOCKET           mSocket;
    static sockaddr_in      mServerAddr;

    static char             mRecieveBuffer[MAX_BUFFER_SIZE];
    static std::vector<Dataform>      mDataTable;
    static char             mServerIP[MAX_BUFFER_SIZE];
    static Dataform         mData;

    // Tests
    static int mStartupTest;
    static int mConnectTest;
    static int mSendTest;


    // while flag
    static bool mbWhileflag;
    static bool mbSwitch;

    static int mbIsConnected;
    static bool mbIsNicknameSet;

    static bool mbIsMsgSent;

};

