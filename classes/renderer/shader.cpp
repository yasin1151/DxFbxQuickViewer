#include "Shader.h"

#include <d3dcompiler.h>

#include "log/easylogging++.h"


Shader::Shader(std::wstring& szVSFilePath, std::wstring& szPSFilePath)
	:m_VSFilePath(szVSFilePath), m_PSFilePath(szPSFilePath)
{
	InitShaderWithFile(szVSFilePath, szPSFilePath);
}

void Shader::UseShader(ID3D11DeviceContext* pContext) const
{
	pContext->VSSetShader(m_VertexShader.Get(), nullptr, 0);
	pContext->PSSetShader(m_PixelShader.Get(), nullptr, 0);
}

HRESULT Shader::CreateShaderFromFile(const WCHAR* hlslFileName, LPCSTR entryPoint,
	LPCSTR shaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;


    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // 设置 D3DCOMPILE_DEBUG 标志用于获取着色器调试信息。该标志可以提升调试体验，
    // 但仍然允许着色器进行优化操作
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // 在Debug环境下禁用优化以避免出现一些不合理的情况
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    ID3DBlob* errorBlob = nullptr;
    hr = D3DCompileFromFile(hlslFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, shaderModel,
        dwShaderFlags, 0, ppBlobOut, &errorBlob);
    if (FAILED(hr))
    {
        if (errorBlob != nullptr)
        {
            OutputDebugStringA(static_cast<const char*>(errorBlob->GetBufferPointer()));
        }
        if (errorBlob)
        {
            errorBlob->Release();
        }
        return hr;
    }

    return hr;
}

bool Shader::InitShaderWithFile(std::wstring& szVSFilePath, std::wstring& szPSFilePath)
{
    HRESULT hr = S_OK;
    hr = CreateShaderFromFile(szVSFilePath.c_str(), "main", "vs_4_0", &m_VertexBlob);
    if (FAILED(hr))
    {
        LOG(ERROR) << "Shader::InitShaderWithFile pVertBlob D3DReadFileToBlob Failed:" << hr;
        return false;
    }


	return true;
}
