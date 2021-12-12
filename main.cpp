#define WIN32_LEAN_AND_MEAN 1

#include <d3d11.h>
#include <d3dx11.h>
#include <iostream>
#include <Windows.h>
#include <log/easylogging++.h>
#include <wrl/client.h>
#include <imgui/imgui.h>

#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11d.lib")


using namespace std;
using namespace Microsoft::WRL;

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

	// dx 相关定义
    ComPtr<ID3D11Device> pDevice;
    ComPtr<ID3D11DeviceContext> pDeviceContext;
    ComPtr<IDXGISwapChain> pSwapChain;
    ComPtr<ID3D11RenderTargetView> pRenderTargetView;
    ComPtr<ID3D11Texture2D> pDepthStencilTexture;
    ComPtr<ID3D11DepthStencilView> pDepthStencilView;
	
};
DeviceData g_DeviceData;

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice(HWND hWnd, HINSTANCE hInstance);
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

    if (FAILED(InitDevice(g_DeviceData.hWnd, hInstance)))
    {
        LOG(ERROR) << "InitDevice Failed :" << GetLastError();
        return 0;
    }

	if (FAILED(InitImgUI(g_DeviceData.hWnd, g_DeviceData.pDevice.Get(), g_DeviceData.pDeviceContext.Get())))
	{
        LOG(ERROR) << "InitImgUI Failed :" << GetLastError();
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

HRESULT InitDevice(HWND hWnd, HINSTANCE hInstance)
{
    HRESULT hr = S_OK;

    RECT rc = {};
    GetClientRect(hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;


    hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                       D3D11_SDK_VERSION, &sd, &g_DeviceData.pSwapChain, &g_DeviceData.pDevice, nullptr, &g_DeviceData.pDeviceContext);
    if (FAILED(hr))
        return hr;

    ComPtr<ID3D11Texture2D> pBackBuffer;
    hr = g_DeviceData.pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer);

    if (FAILED(hr))
        return hr;


    hr = g_DeviceData.pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &g_DeviceData.pRenderTargetView);
    if (FAILED(hr))
        return hr;


    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = g_DeviceData.pDevice->CreateTexture2D(&descDepth, NULL, &g_DeviceData.pDepthStencilTexture);
    if (FAILED(hr))
        return hr;

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_DeviceData.pDevice->CreateDepthStencilView(g_DeviceData.pDepthStencilTexture.Get(), &descDSV, &g_DeviceData.pDepthStencilView);
    if (FAILED(hr))
        return hr;

    g_DeviceData.pDeviceContext->OMSetRenderTargets(1, g_DeviceData.pRenderTargetView.GetAddressOf(), g_DeviceData.pDepthStencilView.Get());


    D3D11_VIEWPORT vp;
    vp.Width = static_cast<FLOAT>(width);
    vp.Height = static_cast<FLOAT>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_DeviceData.pDeviceContext->RSSetViewports(1, &vp);
	
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
    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("FBXViewer");
    ImGui::Text("Test");
    ImGui::End();
    ImGui::Render();

    g_DeviceData.pDeviceContext->ClearRenderTargetView(g_DeviceData.pRenderTargetView.Get(), ClearColor);
    g_DeviceData.pDeviceContext->ClearDepthStencilView(g_DeviceData.pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);


    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    g_DeviceData.pSwapChain->Present(0, 0);

}
