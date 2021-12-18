#include "Application.h"
#include "imgui/imgui.h"
#include "log/easylogging++.h"
#include "renderer/DeviceD3D.h"

const wchar_t* TITLE_NAME = L"FBXQuickViewer";
constexpr unsigned int WINDOW_WIDTH = 800;
constexpr unsigned int WINDOW_HEIGHT = 600;


void Application::Render()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("FBXViewer");
    ImGui::Text("Test");
    ImGui::End();
    ImGui::Render();

    DeviceD3D::GetInstance().Begin(0.0f, 0.125f, 0.3f, 1.0f);

    m_Sprite3D.Draw(DeviceD3D::GetInstance().GetDeviceContext());

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    DeviceD3D::GetInstance().Present();

}

bool Application::Init()
{
    HINSTANCE hInstance = GetModuleHandle(nullptr);

    if (FAILED(this->InitWindow(hInstance, SW_SHOW)))
    {
        LOG(ERROR) << "InitWindow Failed :" << GetLastError();
        return false;
    }

    if (FAILED(DeviceD3D::GetInstance().InitDevice(m_hWnd, hInstance)))
    {
        LOG(ERROR) << "InitDevice Failed :" << GetLastError();
        return false;
    }

    if (FAILED(InitImgUI(m_hWnd, DeviceD3D::GetInstance().GetDevice(), DeviceD3D::GetInstance().GetDeviceContext())))
    {
        LOG(ERROR) << "InitImgUI Failed :" << GetLastError();
        return false;
    }


    // ²âÊÔ¼ÓÔØÄ£ÐÍ

    if (m_Sprite3D.InitWithFile("assets/box.FBX", DeviceD3D::GetInstance().GetDevice()))
    {
        LOG(INFO) << "Load box.FBX Succ, vertices :" << m_Sprite3D.GetAllMeshes()[0]->Vertices.size() << ", indices :" << m_Sprite3D.GetAllMeshes()[0]->Indices.size() << "¡£\n";
    }
    else
    {
        LOG(ERROR) << "Load box.FBX Failed\n";

    }


    return true;
}



HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
#ifdef _DEBUG
    OpenConsole();
#endif

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
    m_hInstnace = hInstance;
    RECT rc = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    m_hWnd = CreateWindow(TITLE_NAME, TITLE_NAME, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
        nullptr);
    if (!m_hWnd)
        return E_FAIL;

    SetWindowLongPtr(m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG>(this));

    ShowWindow(m_hWnd, nCmdShow);


    return S_OK;
}

LRESULT Application::AppWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
	    {
			m_KeyboardInputMgr.OnKeyPressed(static_cast<unsigned char>(wParam));
	    }
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
	    {
			m_KeyboardInputMgr.OnKeyReleased(static_cast<unsigned char>(wParam));
	    }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return false;

}

void Application::OpenConsole()
{
	AllocConsole();
	FILE* pCout;
	freopen_s(&pCout, "CONOUT$", "w", stdout);
	freopen_s(&pCout, "CONOUT$", "w", stderr);
	freopen_s(&pCout, "CONIN$", "r", stdin);
	if (pCout)
		fclose(pCout);
}

HRESULT Application::InitImgUI(HWND hWnd, ID3D11Device* pDeivce, ID3D11DeviceContext* pDeviceContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(pDeivce, pDeviceContext);
	return S_OK;
}

int Application::Run()
{
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


HWND Application::GetHWND() const
{
    return m_hWnd;
}

Application::Application(): m_hWnd(nullptr), m_hInstnace(nullptr)
{

}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	const auto pApp = reinterpret_cast<Application*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    return pApp->AppWndProc(hWnd, message, wParam, lParam);
}
