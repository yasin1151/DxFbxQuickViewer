#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <string>
#include <unordered_map>
#include "AssimpMesh.h"

class Sprite3D
{
public:
	Sprite3D();
	~Sprite3D();

	bool InitWithFile(const std::string& szFilePath, ID3D11Device* pDevice);
	void Draw(ID3D11DeviceContext* pDeviceContext);

	AssimpMesh* GetMesh(std::string& szMeshName);
	std::vector<AssimpMesh*> GetAllMeshes() const;

private:
	void Clear();

	HRESULT InitRenderer(ID3D11Device* pDevice);

private:
	std::unordered_map<std::string, AssimpMesh*> m_Meshes;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer;

	bool m_IsInit;
};

