#pragma once
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <minwinbase.h> // zeroMemory 사용 위함

#define DEFAULT_PORT   8080
#define MAX_BUFFER_SIZE 2048

#define LOCALHOST_IP "127.0.0.1" // 로컬 호스트 주소 (현재 컴퓨터 자체를 가리킴 서버와 클라이언트가 동일한 pc에서 실해오딕 통신 원할때 사용)

class ServerManager
{
public:
    static void initialize();
    static void setServerIP();
    static void createSocket();
    static void convertIP();
    static void connectToServer();
    static void sendMessage();
    static void recieveMessage();
    static void disConnect();

    static void ErrorHandling(const std::wstring& message);


public:
    static WSADATA          mWSdata;
    static WORD             mWSVersion;
    static SOCKET           mSocket;
    static sockaddr_in      mServerAddr;

    static char             mTextRecieveBuffer[MAX_BUFFER_SIZE];
    static std::string      mServerIP;
    static std::string      mMessage;

    // Tests
    static int mStartupTest;
    static int mConnectTest;
    static int mSendTest;
    static int mRecieveTest;

};

