#include <iostream>
#include "ServerManager.h"


int main()
{

    // �̴ϼȶ�����
    ServerManager::initialize();

    // ���� ���� ����
    ServerManager::setServerIP();

    // ���� ����
    ServerManager::createSocket(ServerManager::mSocket);
    std::cout << "Sockets are created.\n";

    // ip ����Ʈ
    ServerManager::convertIP();

    // ����
    ServerManager::connectToServer();

    // �޽��� �ۼ���
    // �÷��̾� ���� ä�� �ۼ��� ���� �ٸ������忡�� �����ؾ���


    std::thread sendThread(ServerManager::sendMessage, ePacketType::Message, false);
    sendThread.detach();

    std::thread receiveThread(ServerManager::receiveMessage, false);
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

    // ���� ���� ����
    ServerManager::disConnect();

    return 0;
}

