#pragma once
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <minwinbase.h> // zeroMemory ��� ����

#define DEFAULT_PORT   8080
#define MAX_BUFFER_SIZE 2048

#define LOCALHOST_IP "127.0.0.1" // ���� ȣ��Ʈ �ּ� (���� ��ǻ�� ��ü�� ����Ŵ ������ Ŭ���̾�Ʈ�� ������ pc���� ���ؿ��� ��� ���Ҷ� ���)
#define ID_SIZE   20

struct Dataform
{
    std::string      name;
    std::string      message;
};

class ServerManager
{
public:
    static void initialize();
    static void setServerIP();
    static void createSocket();
    static void convertIP();
    static void connectToServer();
    static void chatSend();
    static void chatReceive();
    static void disConnect();

    static void ErrorHandling(const std::wstring& message);
    static void sendMessage(std::string _Message);
    static void sendData();

    static void receiveMessage();
    static void receiveData();


public:
    static WSADATA          mWSdata;
    static WORD             mWSVersion;
    static SOCKET           mSocket;
    static sockaddr_in      mServerAddr;

    static char             mTextRecieveBuffer[MAX_BUFFER_SIZE];
    static std::string      mServerIP;
    static Dataform         mData;

    // Tests
    static int mStartupTest;
    static int mConnectTest;
    static int mSendTest;
    static int mReceiveTest;

};

