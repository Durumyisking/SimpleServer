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
        L"ä�� ���α׷�", // lpszClassName, L-string
        NULL
    };

    RegisterClassEx(&wc);

    // ������ �׷����� �ػ󵵸� �����ϱ� ����
    RECT wr = { 0, 0, width, height };    // set the size, but not the position
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);    // adjust the size

    HWND hwnd = CreateWindow(
        wc.lpszClassName,
        L"ä�� ���α׷�",
        WS_OVERLAPPEDWINDOW,
        100, // ������ ���� ����� x ��ǥ
        100, // ������ ���� ����� y ��ǥ
        wr.right - wr.left, // ������ ���� ���� �ػ�
        wr.bottom - wr.top, // ������ ���� ���� �ػ�
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
        std::cout << "GUI Dx11 �ʱ�ȭ ����!!" << std::endl;
        return 0;
    }
    if (!ImGui_ImplWin32_Init(hwnd))
    {
        std::cout << "GUI Win32 �ʱ�ȭ ����!!" << std::endl;
        return 0;   
    }

    // ���� �̴ϼȶ�����
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
            ImGui_ImplDX11_NewFrame();//TODO: IMGUI ���
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin(u8"ä��ä��");

            UpdateGUI();
            ImGui::End();
            ImGui::Render();
            application->Update();
            application->Render();

            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());//TODO: IMGUI ���

            // switch the back buffer and the front buffer
            application->swapChain->Present(1, 0);
        }
    }

    // ���� ���� ����
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
        // ip����
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
            for (Dataform dataform : ServerManager::mDataTable)
            {
                ImGui::Text(dataform.name);
                ImGui::SameLine();
                ImGui::Text(dataform.message);
            }
            
            ImGui::InputTextWithHint("##SendMsgTxtBox", u8"�޽����� �Է����ּ���", ServerManager::mData.message, MSG_SIZE);
            ImGui::SameLine();
            if (ImGui::Button(u8"����"))
            {
                ServerManager::mbIsMsgSent = true;
            }
        }

    }

}
