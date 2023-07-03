#include <iostream>
#include "ServerManager.h"


int main()
{

    // 이니셜라이즈
    ServerManager::initialize();

    // 서버 정보 설정
    ServerManager::setServerIP();

    // 소켓 생성
    ServerManager::createSocket(ServerManager::mJoinSocket);
    ServerManager::createSocket(ServerManager::mSocket);


    // ip 컨버트
    ServerManager::convertIP();

    // 연결
    ServerManager::connectToServer();

    // 메시지 송수신
    // 플레이어 입장 채팅 송수신 전부 다른쓰레드에서 동작해야함



    while (ServerManager::mbWhileflag)
    {
        std::thread joinThread(ServerManager::joinReceive);
        joinThread.detach();

        std::thread sendThread(ServerManager::chatSend);
        sendThread.detach();

        std::thread receiveThread(ServerManager::chatReceive);
        receiveThread.detach();

        // 여기서 서버가 닫혔는지 클라이언트가 종료할건지 말건지 판단한다
        while (true)
        {
            if (!ServerManager::mbSwitch)
            {
                ServerManager::mbWhileflag = false;
                break;
            }
        }
    }

    // 서버 연결 해제
    ServerManager::disConnect();

    return 0;
}

