#pragma once
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <minwinbase.h> // zeroMemory 사용 위함
#include <string>
#include <stdio.h>
#include <thread>

#define DEFAULT_PORT   8080
#define MAX_BUFFER_SIZE 2048

#define IP_SIZE 20
#define LOCALHOST_IP "127.0.0.1" // 로컬 호스트 주소 (현재 컴퓨터 자체를 가리킴 서버와 클라이언트가 동일한 pc에서 실해오딕 통신 원할때 사용)
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

    static void chatReceive();

    static void disConnect();

    static void ErrorHandling(const std::wstring& message);
    static void sendMessage(ePacketType packetType);
    static void receiveMessage();

private:
    static void makeConnection(SOCKET _Socket, sockaddr_in _ServerAddr);


public:
    static WSADATA          mWSdata;
    static WORD             mWSVersion;
    static SOCKET           mSocket;
    static sockaddr_in      mServerAddr;

    static char             mRecieveBuffer[MAX_BUFFER_SIZE];
    static char             mServerIP[MAX_BUFFER_SIZE];
    static Dataform         mData;

    // Tests
    static int mStartupTest;
    static int mConnectTest;
    static int mSendTest;


    // while flag
    static bool mbWhileflag;
    static bool mbSwitch;


};

