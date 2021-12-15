#define WIN32_LEAN_AND_MEAN 1

#include <d3d11.h>
#include <d3dx11.h>
#include <Windows.h>
#include <log/easylogging++.h>

#include "Application.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11d.lib")
#pragma comment(lib, "assimp-vc142-mtd.lib")

// ≥ı ºªØ easylogging
INITIALIZE_EASYLOGGINGPP

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

    Application app;
    if (!app.Init())
    {
        return -1;
    }

    LOG(INFO) << "Init Success!!! \n" ;

    return app.Run();
}

