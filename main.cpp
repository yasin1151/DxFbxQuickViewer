#define WIN32_LEAN_AND_MEAN 1

#include <Windows.h>
#include <iostream>
#include <log/easylogging++.h>

using namespace std;

// 标题名字，宽高
const wchar_t* TITLE_NAME = L"FBXQuickViewer";
const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;


// 初始化 easylogging
INITIALIZE_EASYLOGGINGPP

struct DeviceData
{
    HWND hWnd;
    HINSTANCE hInstnace;
};
DeviceData g_DeviceData;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = TITLE_NAME;
    if (!RegisterClassEx(&wc))
        return E_FAIL;

    // Create window
    g_DeviceData.hInstnace = hInstance;
    RECT rc = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    g_DeviceData.hWnd = CreateWindow(TITLE_NAME, TITLE_NAME, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
        nullptr);
    if (!g_DeviceData.hWnd)
        return E_FAIL;

    ShowWindow(g_DeviceData.hWnd, nCmdShow);

    return S_OK;
}

void OpenConsole()
{
    AllocConsole();
    FILE* pCout;
    freopen_s(&pCout, "CONOUT$", "w", stdout);
    freopen_s(&pCout, "CONOUT$", "w", stderr);
    freopen_s(&pCout, "CONIN$", "r", stdin);
    if (pCout)
        fclose(pCout);
}


void Render()
{

}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

    ZeroMemory(&g_DeviceData, sizeof(g_DeviceData));

    // debug模式下打开控制台
#ifdef _DEBUG
    OpenConsole();
#endif

	
    if (FAILED(InitWindow(hInstance, nCmdShow)))
    {
        LOG(ERROR) << "InitWindow Failed :" << GetLastError();
        return 0;
    }

    LOG(INFO) << "Init Success!!! \n" ;
	
    MSG msg = { nullptr };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Render();
        }
    }

    return static_cast<int>(msg.wParam);
}



