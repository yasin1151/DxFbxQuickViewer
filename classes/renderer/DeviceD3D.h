#pragma once
#include <d3d11.h>
#include <wrl/client.h>


class DeviceD3D
{
public:
    HRESULT InitDevice(HWND hWnd, HINSTANCE hInstance);

    ID3D11Device* GetDevice() const;
    ID3D11DeviceContext* GetDeviceContext() const;

    void Begin(float r, float g, float b, float a) const;
    void Present() const;

    static DeviceD3D& GetInstance();

private:
    DeviceD3D();

	Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_DeviceContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_DepthStencilTexture;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DepthStencilView;
};
