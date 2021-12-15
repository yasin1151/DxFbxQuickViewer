#define WIN32_LEAN_AND_MEAN 1

#include <d3d11.h>
#include <d3dx11.h>
#include <iostream>
#include <Windows.h>
#include <log/easylogging++.h>
#include <wrl/client.h>
#include <imgui/imgui.h>

#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

#include "modelload/AssimpLoader.h"
#include "modelload/Sprite3D.h"
#include "renderer/DeviceD3D.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11d.lib")
#pragma comment(lib, "assimp-vc142-mtd.lib")

using namespace std;
using namespace Microsoft::WRL;

// 标题名字，宽高
const wchar_t* TITLE_NAME = L"FBXQuickViewer";
constexpr unsigned int WINDOW_WIDTH = 800;
constexpr unsigned int WINDOW_HEIGHT = 600;


// 初始化 easylogging
INITIALIZE_EASYLOGGINGPP

struct DeviceData
{
    HWND hWnd;
    HINSTANCE hInstnace;
};
DeviceData g_DeviceData;
Sprite3D g_Sprite3D;

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitImgUI(HWND hWnd, ID3D11Device* pDeivce, ID3D11DeviceContext* pDeviceContext);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void Render();

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

    if (FAILED(DeviceD3D::GetInstance().InitDevice(g_DeviceData.hWnd, hInstance)))
    {
        LOG(ERROR) << "InitDevice Failed :" << GetLastError();
        return 0;
    }

	if (FAILED(InitImgUI(g_DeviceData.hWnd, DeviceD3D::GetInstance().GetDevice(), DeviceD3D::GetInstance().GetDeviceContext())))
	{
        LOG(ERROR) << "InitImgUI Failed :" << GetLastError();
        return 0;
	}

    LOG(INFO) << "Init Success!!! \n" ;

	// 测试加载模型

    if (g_Sprite3D.InitWithFile("assets/box.FBX", DeviceD3D::GetInstance().GetDevice()))
    {
        LOG(INFO) << "Load box.FBX Succ, vertices :" << g_Sprite3D.GetAllMeshes()[0]->Vertices.size() << ", indices :" << g_Sprite3D.GetAllMeshes()[0]->Indices.size() << "。\n";
    }
    else
    {
        LOG(ERROR) << "Load box.FBX Failed\n";

    }
	
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

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

	
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return false;
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

HRESULT InitImgUI(HWND hWnd, ID3D11Device* pDeivce, ID3D11DeviceContext* pDeviceContext)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(pDeivce, pDeviceContext);
    return S_OK;
}

void Render()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("FBXViewer");
    ImGui::Text("Test");
    ImGui::End();
    ImGui::Render();

    DeviceD3D::GetInstance().Begin(0.0f, 0.125f, 0.3f, 1.0f);

    g_Sprite3D.Draw(DeviceD3D::GetInstance().GetDeviceContext());

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    DeviceD3D::GetInstance().Present();

}
