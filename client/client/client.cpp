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
        ServerManager::sendMessage();

        ServerManager::recieveMessage();
    }

    // ���� ���� ����
    ServerManager::disConnect();

    return 0;
}

