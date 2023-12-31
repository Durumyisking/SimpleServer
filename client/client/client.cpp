#include "Application.h"

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void UpdateGUI();

int main()
{

    const int width = 1280, height = 960;
    const int canvasWidth = width / 80, canvasHeight = height / 80;

    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX),
        CS_CLASSDC,
        WndProc,
        0L,
        0L,
        GetModuleHandle(NULL),
        NULL,
        NULL,
        NULL,
        NULL,
        L"채팅 프로그램", // lpszClassName, L-string
        NULL
    };

    RegisterClassEx(&wc);

    // 실제로 그려지는 해상도를 설정하기 위해
    RECT wr = { 0, 0, width, height };    // set the size, but not the position
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);    // adjust the size

    HWND hwnd = CreateWindow(
        wc.lpszClassName,
        L"채팅 프로그램",
        WS_OVERLAPPEDWINDOW,
        100, // 윈도우 좌측 상단의 x 좌표
        100, // 윈도우 좌측 상단의 y 좌표
        wr.right - wr.left, // 윈도우 가로 방향 해상도
        wr.bottom - wr.top, // 윈도우 세로 방향 해상도
        NULL,
        NULL,
        wc.hInstance,
        NULL);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    auto application = std::make_unique<Application>(hwnd, width, height, canvasWidth, canvasHeight);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.DisplaySize = ImVec2(width, height);
    //io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/malgun.ttf", 18.f, NULL, io.Fonts->GetGlyphRangesKorean());
    io.Fonts->Build();
    ImGui::StyleColorsLight();

    if (!ImGui_ImplDX11_Init(application->device, application->deviceContext))
    {
        std::cout << "GUI Dx11 초기화 실패!!" << std::endl;
        return 0;
    }
    if (!ImGui_ImplWin32_Init(hwnd))
    {
        std::cout << "GUI Win32 초기화 실패!!" << std::endl;
        return 0;   
    }

    // 서버 이니셜라이즈
    ServerManager::initialize();

    // Main message loop
    MSG msg = {};
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // Start the Dear ImGui frame
            ImGui_ImplDX11_NewFrame();//TODO: IMGUI 사용
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin(u8"채팅채팅");

            UpdateGUI();
            ImGui::End();
            ImGui::Render();
            application->Update();
            application->Render();

            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());//TODO: IMGUI 사용

            // switch the back buffer and the front buffer
            application->swapChain->Present(1, 0);
        }
    }

    // 서버 연결 해제
    ServerManager::disConnect();

    // Cleanup
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Windows procedure
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        // Reset and resize swapchain
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu 
            return 0;
        break;
    case WM_MOUSEMOVE:
        //std::cout << "Mouse " << LOWORD(lParam) << " " << HIWORD(lParam) << std::endl;
        break;
    case WM_LBUTTONUP:
        //std::cout << "WM_LBUTTONUP Left mouse button" << std::endl;
        break;
    case WM_RBUTTONUP:
        //std::cout << "WM_RBUTTONUP Right mouse button" << std::endl;
        break;
    case WM_KEYDOWN:
        //std::cout << "WM_KEYDOWN " << (int)wParam << std::endl;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

void UpdateGUI()
{
    ImGui::Text("Average %.3f ms/frame (%.1f FPS)",
        1000.0f / ImGui::GetIO().Framerate,
        ImGui::GetIO().Framerate);

    if (!ServerManager::mbIsConnected || ServerManager::mbIsConnected == -1)
    {
        // ip설정
        ServerManager::setServerIP();
    }
    else
    {
        if (!ServerManager::mbIsNicknameSet)
        {
            ServerManager::setUserNickName();
        }
        else
        {
            for (const Dataform& dataform : ServerManager::mDataTable)
            {
                ImVec4 txtcolor = {};
                if (!strcmp(ServerManager::mData.name, dataform.name))
                {
                    txtcolor = {0.f, 1.f, 0.f, 1.f};
                }
                else
                {
                    txtcolor = { 1.f, 0.f, 1.f, 1.f };
                }

                if (dataform.PacketType == ePacketType::UserJoin)
                {
                    ImGui::TextColored(txtcolor, dataform.name);
                    ImGui::SameLine();
                    ImGui::Text(u8" 님이 입장하셨습니다.");
                }
                else if (dataform.PacketType == ePacketType::Message)
                {
                    ImGui::TextColored(txtcolor, dataform.name);
                    ImGui::SameLine();
                    ImGui::Text(" : ");
                    ImGui::SameLine();
                    ImGui::Text(dataform.message);
                }

            }
            
            ImGui::InputTextWithHint("##SendMsgTxtBox", u8"메시지를 입력해주세요", ServerManager::mData.message, MSG_SIZE);
            ImGui::SameLine();
            if (ImGui::Button(u8"전송") || (ImGui::IsKeyPressed(ImGuiKey_Enter) ))
            {
                ServerManager::mbIsMsgSent = true;
            }
        }

    }

}
