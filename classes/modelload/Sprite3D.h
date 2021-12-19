#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <string>
#include <unordered_map>
#include <DirectXMath.h>

#include "AssimpMesh.h"

class Sprite3D
{
public:
	Sprite3D();
	~Sprite3D();

	bool InitWithFile(const std::string& szFilePath, ID3D11Device* pDevice);
	void Draw(ID3D11DeviceContext* pDeviceContext, DirectX::XMMATRIX viewMat, DirectX::XMMATRIX projectMat);

	AssimpMesh* GetMesh(std::string& szMeshName);
	std::vector<AssimpMesh*> GetAllMeshes() const;

	Vec3 GetPos() const;
	void SetPos(Vec3 pos);
	void Translate(Vec3 deltaPos);


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

	Vec3 m_Pos;

	bool m_IsInit;
};

