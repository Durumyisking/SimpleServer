#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

#define DEFAULT_PORT   5150
#define DEFAULT_BUFFER 2048

void ErrorHandling(const std::wstring& message);

int main()
{
    WSADATA wsd;
    SOCKET sClient;
    char szBuffer[DEFAULT_BUFFER];
    char szServerIP[128] = { 0 };
    char szMessage[1024];

    int ret;
    int sendret;
    struct sockaddr_in serverAddr;

    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
    {
        ErrorHandling(L"WSAStartup() error!");
    }
    else
    {
        puts("The library has been initialized.\n\n");
    }

    sClient = socket(AF_INET, SOCK_STREAM, 0);
    if (sClient == INVALID_SOCKET)
    {
        ErrorHandling(L"socket() error");
    }
    else
    {
        puts("Socket has been created.\n");
    }

    puts("Input Connect Server IP: ");
    fgets(szServerIP, 128, stdin);
    szServerIP[strcspn(szServerIP, "\n")] = '\0';

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(DEFAULT_PORT);
    inet_ntop(AF_INET, &serverAddr.sin_addr, szServerIP, sizeof(szServerIP));

    if (connect(sClient, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        ErrorHandling(L"connect() error!");
    }
    else
    {
        puts("Connected to the server.\n");
    }

    while (1)
    {
        printf("Input SendMessage: ");
        std::cin >> szMessage;

        sendret = send(sClient, szMessage, strlen(szMessage), 0);
        if (sendret == SOCKET_ERROR)
        {
            ErrorHandling(L"send() error!");
        }

        ret = recv(sClient, szBuffer, DEFAULT_BUFFER, 0);
        if (ret == SOCKET_ERROR)
        {
            ErrorHandling(L"recv() error!");
        }
        else
        {
            szBuffer[ret] = '\0';
            printf("Received Message: %s\n", szBuffer);
        }
    }

    closesocket(sClient);

    WSACleanup();

    return 0;
}

void ErrorHandling(const std::wstring& message)
{
    std::wcout << message << std::endl;
    exit(1);
}