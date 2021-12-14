#pragma once
#include <d3d11.h>
#include <string>
#include <wrl/client.h>


class Shader
{
public:
	Shader(std::wstring& szVSFilePath, std::wstring& szPSFilePath);

	void UseShader(ID3D11DeviceContext* pContext) const;


	static HRESULT CreateShaderFromFile(const WCHAR* hlslFileName,
		LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** ppBlobOut);

private:

	bool InitShaderWithFile(std::wstring& szVSFilePath, std::wstring& szPSFilePath);

	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> m_PixelBlob;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> m_VertexBlob;


	std::wstring& m_VSFilePath;
	std::wstring& m_PSFilePath;

};
