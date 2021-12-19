#pragma once
#include <cstdio>
#include <Windows.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "modelload/Sprite3D.h"
#include "input/KeyboardInput.h"
#include "renderer/Camera.h"


class Application
{
public:
	Application();

	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);

	LRESULT AppWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static HRESULT InitImgUI(HWND hWnd, ID3D11Device* pDeivce, ID3D11DeviceContext* pDeviceContext);

	bool Init();

	int Run();

	void HandleInput();

	void Render();

	HWND GetHWND() const;

private:
	void OpenConsole();


private:

	HWND m_hWnd;
	HINSTANCE m_hInstnace;
	KeyboardInput m_KeyboardInputMgr;
	Camera m_Camera;

	Sprite3D m_Sprite3D;
};


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
