#include <iostream>
#include "ServerManager.h"


int main()
{

    // �̴ϼȶ�����
    ServerManager::initialize();

    // ���� ���� ����
    ServerManager::setServerIP();

    // ���� ����
    ServerManager::createSocket();


    // ip ����Ʈ
    ServerManager::convertIP();

    // ����
    ServerManager::connectToServer();

    // �޽��� �ۼ���
    while (1)
    {
        ServerManager::chatSend();

        ServerManager::chatReceive();
    }

    // ���� ���� ����
    ServerManager::disConnect();

    return 0;
}

