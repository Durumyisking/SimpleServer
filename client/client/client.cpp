#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <minwinbase.h>

#define DEFAULT_PORT   8080
#define MAX_BUFFER_SIZE 2048

void ErrorHandling(const std::wstring& message);

int main()
{
    WSADATA wsd;
    WORD version;
    SOCKET clientSocket;
    char szBuffer[MAX_BUFFER_SIZE];
    std::string serverIP;
    std::string Message;

    int ret;
    int sendret;
    struct sockaddr_in serverAddr;

    // �̴ϼȶ�����
    version = MAKEWORD(2, 2);
    int wsResult = WSAStartup(version, &wsd);
    if (wsResult != 0)
    {
        ErrorHandling(L"WSAStartup() error!");
    }
    else
    {
        std::cout << "The library has been initialized.\n" ;
    }

    // ���� ���� ����
    //std::cout << "Input Connect Server IP: ";
    //std::cin >> serverIP;
    // 127.0.0.1 = ���� ȣ��Ʈ �ּ� (���� ��ǻ�� ��ü�� ����Ŵ ������ Ŭ���̾�Ʈ�� ������ pc���� ���ؿ��� ��� ���Ҷ� ���)
    serverIP = "127.0.0.1"; 
    int serverPort = DEFAULT_PORT;       // ���� ��Ʈ ��ȣ

    // ���� ����
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        ErrorHandling(L"socket() error");
    }
    else
    {
        std::cout<< "Socket has been created.\n";
    }


    // ip ����Ʈ
//    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, serverIP.c_str(), &(serverAddr.sin_addr));

    // ����
    int connResult = connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
    if (connResult == SOCKET_ERROR)
    {
        closesocket(clientSocket);
        ErrorHandling(L"connect() error!");
    }
    else
    {
        std::cout<<"Connected to the server.\n";
    }

    while (1)
    {
        std::cout << "Input SendMessage: ";
        std::cin >> Message;

        sendret = send(clientSocket, Message.c_str(), Message.size() + 1, 0);
        if (sendret == SOCKET_ERROR)
        {
            ErrorHandling(L"send() error!");
        }

        ZeroMemory(szBuffer, MAX_BUFFER_SIZE);
        ret = recv(clientSocket, szBuffer, MAX_BUFFER_SIZE, 0);
        if (ret == SOCKET_ERROR)
        {
            ErrorHandling(L"recv() error!");
        }
        else
        {
            std::cout << "Received Message : " << szBuffer << std::endl;;
        }
    }

    closesocket(clientSocket);

    WSACleanup();

    return 0;
}

void ErrorHandling(const std::wstring& message)
{
    std::wcout << message << std::endl;
    WSACleanup();
    exit(1);
}