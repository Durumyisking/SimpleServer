#include <iostream>
#include "ServerManager.h"


int main()
{

    // �̴ϼȶ�����
    ServerManager::initialize();

    // ���� ���� ����
    ServerManager::setServerIP();

    // ���� ����
    ServerManager::createSocket(ServerManager::mJoinSocket);
    ServerManager::createSocket(ServerManager::mSocket);


    // ip ����Ʈ
    ServerManager::convertIP();

    // ����
    ServerManager::connectToServer();

    // �޽��� �ۼ���
    // �÷��̾� ���� ä�� �ۼ��� ���� �ٸ������忡�� �����ؾ���



    while (ServerManager::mbWhileflag)
    {
        std::thread joinThread(ServerManager::joinReceive);
        joinThread.detach();

        std::thread sendThread(ServerManager::chatSend);
        sendThread.detach();

        std::thread receiveThread(ServerManager::chatReceive);
        receiveThread.detach();

        // ���⼭ ������ �������� Ŭ���̾�Ʈ�� �����Ұ��� ������ �Ǵ��Ѵ�
        while (true)
        {
            if (!ServerManager::mbSwitch)
            {
                ServerManager::mbWhileflag = false;
                break;
            }
        }
    }

    // ���� ���� ����
    ServerManager::disConnect();

    return 0;
}

