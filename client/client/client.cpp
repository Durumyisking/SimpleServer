#include <iostream>
#include "ServerManager.h"


int main()
{

    // 이니셜라이즈
    ServerManager::initialize();

    // 서버 정보 설정
    ServerManager::setServerIP();

    // 소켓 생성
    ServerManager::createSocket();


    // ip 컨버트
    ServerManager::convertIP();

    // 연결
    ServerManager::connectToServer();

    // 메시지 송수신
    while (1)
    {
        ServerManager::chatSend();

        ServerManager::chatReceive();
    }

    // 서버 연결 해제
    ServerManager::disConnect();

    return 0;
}

