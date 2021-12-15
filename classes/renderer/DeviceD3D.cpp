#include "DeviceD3D.h"

DeviceD3D::DeviceD3D()
{

}

HRESULT DeviceD3D::InitDevice(HWND hWnd, HINSTANCE hInstance)
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
        D3D11_SDK_VERSION, &sd, &m_SwapChain, &m_Device, nullptr, &m_DeviceContext);
    if (FAILED(hr))
        return hr;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
    hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer);

    if (FAILED(hr))
        return hr;


    hr = m_Device->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &m_RenderTargetView);
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
    hr = m_Device->CreateTexture2D(&descDepth, nullptr, &m_DepthStencilTexture);
    if (FAILED(hr))
        return hr;

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = m_Device->CreateDepthStencilView(m_DepthStencilTexture.Get(), &descDSV, &m_DepthStencilView);
    if (FAILED(hr))
        return hr;

    m_DeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());


    D3D11_VIEWPORT vp;
    vp.Width = static_cast<FLOAT>(width);
    vp.Height = static_cast<FLOAT>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_DeviceContext->RSSetViewports(1, &vp);

    return hr;
}

ID3D11Device* DeviceD3D::GetDevice() const
{
    return m_Device.Get();
}

ID3D11DeviceContext* DeviceD3D::GetDeviceContext() const
{
    return m_DeviceContext.Get();
}

void DeviceD3D::Begin(float r, float g, float b, float a) const
{
	const float color[] = { r, g, b, a };
    m_DeviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), color);
    m_DeviceContext->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void DeviceD3D::Present() const
{
    m_SwapChain->Present(0, 0);
}

DeviceD3D& DeviceD3D::GetInstance()
{
    static DeviceD3D instance;
    return instance;
}
