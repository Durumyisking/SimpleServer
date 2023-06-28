#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <thread>

#define DEFAULT_PORT   5150
#define DEFAULT_BUFFER 2048

void ErrorHandling(const std::wstring& message);
void HandleClient(SOCKET clientSocket);

int main()
{
    WSADATA wsd;
    SOCKET sListen;
    std::vector<SOCKET> clientSockets;

    struct sockaddr_in local;

    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
    {
        ErrorHandling(L"WSAStartup() error!");
    }
    else
    {
        puts("The library has been initialized.\n\n");
    }

    sListen = socket(AF_INET, SOCK_STREAM, 0);
    if (sListen == INVALID_SOCKET)
    {
        ErrorHandling(L"socket() error!");
    }
    else
    {
        std::cout << "Server socket created.\n" << std::endl;
    }

    memset(&local, 0, sizeof(local));
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_family = AF_INET;
    local.sin_port = htons(DEFAULT_PORT);

    if (bind(sListen, (struct sockaddr*)&local, sizeof(local)) == SOCKET_ERROR)
    {
        ErrorHandling(L"bind() error!");
    }

    listen(sListen, SOMAXCONN);

    while (1)
    {
        struct sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);

        SOCKET clientSocket = accept(sListen, (struct sockaddr*)&clientAddr, &clientAddrSize);

        if (clientSocket == INVALID_SOCKET)
        {
            ErrorHandling(L"accept() error!");
        }
        else
        {
            char clientIP[20] = { 0 };
            if (NULL == inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP))) // inet_ntop ip 주소를 문자열 형태로 변환
            {
                ErrorHandling(L"inet_ntop() error!");
            }
            else
            {
                std::cout << "[Client IP: " << clientIP << ", PORT:" << ntohs(clientAddr.sin_port) << "]" << std::endl;
//                printf("[Client IP:%s, PORT: %d\n", clientIP, ntohs(clientAddr.sin_port)); // ntohs 포트넘버를 정수 형태로 변환
                clientSockets.push_back(clientSocket);

                std::thread clientThread(HandleClient, clientSocket);
                clientThread.detach();
            }
        }
    }

    // 연결된 클라이언트 소켓을 닫음
    for (SOCKET clientSocket : clientSockets)
    {
        closesocket(clientSocket);
    }

    closesocket(sListen);

    WSACleanup();

    return 0;
}

void HandleClient(SOCKET clientSocket)
{
    char szBuffer[DEFAULT_BUFFER];
    int ret = 0;
    int sendret = 0;

    while (1)
    {
        ret = recv(clientSocket, szBuffer, DEFAULT_BUFFER, 0);
        if (ret == SOCKET_ERROR)
        {
            ErrorHandling(L"recv() error!");
            break;
        }
        else if (ret == 0)
        {
            std::cout << "Client disconnected.\n" << std::endl;
            break;
        }
        else
        {
            szBuffer[ret] = '\0';
            std::cout << "Received message from client:" << szBuffer << std::endl;

            sendret = send(clientSocket, szBuffer, ret, 0);
            if (sendret == SOCKET_ERROR)
            {
                ErrorHandling(L"send() error!");
                break;
            }
        }
    }

    closesocket(clientSocket);
}

void ErrorHandling(const std::wstring& message)
{
    std::wcout << message << std::endl;
    exit(1);
}