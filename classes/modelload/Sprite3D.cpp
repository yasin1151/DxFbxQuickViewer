#include "Sprite3D.h"
#include "log/easylogging++.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <DirectXMath.h>

#include "Matrix.h"
#include "renderer/shader.h"


static const char* szVertShader = R"delimiter(
struct VertexIn
{
	float4 Position : POSITION;
	float4 Normal : NORMAL;
};

struct PixelIn
{
	float4 Position : SV_POSITION;
	float4 WorldPosition : POSITION;
	float4 Normal : NORMAL;
};

cbuffer PerFrame : register(b0)
{
	matrix mModel;
	matrix mView;
	matrix mProj;
};

PixelIn main(VertexIn vin)
{
	PixelIn vout;

	vout.Position = mul(vin.Position, mModel);
	vout.Position = mul(vout.Position, mView);
	vout.Position = mul(vout.Position, mProj);

	vout.WorldPosition = mul(vin.Position, mModel);

	vout.Normal = mul(vin.Normal, mModel);

	return vout;
}
)delimiter";


struct MVP_MAT
{
	DirectX::XMMATRIX Model;
	DirectX::XMMATRIX View;
	DirectX::XMMATRIX Proj;
};


static const char* szPixelShader = R"delimiter(
struct PixelIn
{
	float4 Position : SV_POSITION;
	float4 WorldPosition : POSITION;
	float4 Normal : NORMAL;
};

float4 main(PixelIn pin) : SV_TARGET
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
)delimiter";


Sprite3D::Sprite3D()
	:m_IsInit(false)
{
	
}

Sprite3D::~Sprite3D()
{
	this->Clear();
}

bool Sprite3D::InitWithFile(const std::string& szFilePath, ID3D11Device* pDevice)
{
	m_IsInit = false;
	const aiScene* pAIScene = aiImportFile(szFilePath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

	if (!pAIScene)
	{
		LOG(ERROR) << "AssimpLoader aiImportFile Failed \n";
		return false;
	}

	const unsigned int iNumMeshes = pAIScene->mNumMeshes;
	this->Clear();
	m_Meshes.reserve(iNumMeshes);
	for (int meshIndex = 0; meshIndex < iNumMeshes; meshIndex++)
	{

		const aiMesh* pAIMesh = pAIScene->mMeshes[meshIndex];
		AssimpMesh* pAssimpMesh = new AssimpMesh();
		m_Meshes[pAIMesh->mName.C_Str()] = pAssimpMesh;

		pAssimpMesh->Vertices.reserve(pAIMesh->mNumVertices);
		for (int vertIndex = 0; vertIndex < pAIMesh->mNumVertices; vertIndex++)
		{
			aiVector3D vert = pAIMesh->mVertices[vertIndex];
			aiVector3D normal = pAIMesh->mNormals[vertIndex];

			pAssimpMesh->Vertices.emplace_back(
				Vec3(vert.x, vert.y, vert.z),
				Vec3(normal.x, normal.y, normal.z));
		}

		pAssimpMesh->Indices.reserve(pAIMesh->mNumFaces * 3);
		for (int faceIndex = 0; faceIndex < pAIMesh->mNumFaces; faceIndex++)
		{
			pAssimpMesh->Indices.push_back(pAIMesh->mFaces[faceIndex].mIndices[0]);
			pAssimpMesh->Indices.push_back(pAIMesh->mFaces[faceIndex].mIndices[1]);
			pAssimpMesh->Indices.push_back(pAIMesh->mFaces[faceIndex].mIndices[2]);
		}

		pAssimpMesh->InitBuffer(pDevice);

	}

	if (FAILED(InitRenderer(pDevice)))
	{
		return false;
	}
	m_IsInit = true;
	return true;
}

void Sprite3D::Draw(ID3D11DeviceContext* pDeviceContext, DirectX::XMMATRIX viewMat, DirectX::XMMATRIX projectMat)
{
	if (!m_IsInit)
		return;

	for (const auto it : m_Meshes)
	{
		auto* pMesh = it.second;

		pDeviceContext->IASetInputLayout(m_InputLayout.Get());
		pDeviceContext->VSSetShader(m_VertexShader.Get(), nullptr, 0);
		pDeviceContext->PSSetShader(m_PixelShader.Get(), nullptr, 0);

		D3D11_MAPPED_SUBRESOURCE vscb;
		HRESULT hr = pDeviceContext->Map(m_VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0x00, &vscb);
		if (FAILED(hr))
		{
			LOG(ERROR) << "Sprite3D::Draw Map Failed:" << hr;
			return;
		}

		auto worldMat = DirectX::XMMatrixIdentity();

		MVP_MAT mvp = { worldMat , viewMat , projectMat };

		memcpy(vscb.pData, &mvp, sizeof(mvp));
		pDeviceContext->Unmap(m_VertexBuffer.Get(), 0);

		pDeviceContext->VSSetConstantBuffers(0, 1, m_VertexBuffer.GetAddressOf());


		std::uint32_t stride = sizeof(AssimpMesh::AssimpVertex);
		std::uint32_t offset = 0u;

		pDeviceContext->IASetVertexBuffers(0, 1, pMesh->VertexBuffer.GetAddressOf(), &stride, &offset);

		pDeviceContext->IASetIndexBuffer(pMesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);

		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		pDeviceContext->DrawIndexed(pMesh->NumberOfIndices, 0, 0);

	}

}

AssimpMesh* Sprite3D::GetMesh(std::string& szMeshName)
{
	if (m_Meshes.find(szMeshName) != m_Meshes.end())
	{
		return m_Meshes[szMeshName];
	}
	return nullptr;
}

std::vector<AssimpMesh*> Sprite3D::GetAllMeshes() const
{
	std::vector<AssimpMesh*> ret;
	for (const auto& it : m_Meshes)
	{
		ret.push_back(it.second);
	}
	return ret;
}

Vec3 Sprite3D::GetPos() const
{
	return m_Pos;
}

void Sprite3D::SetPos(Vec3 pos)
{
	m_Pos = pos;
}

void Sprite3D::Translate(Vec3 deltaPos)
{
	m_Pos += deltaPos;
}

void Sprite3D::Clear()
{
	if (m_Meshes.empty())
	{
		return;
	}

	for (auto it : m_Meshes)
	{
		delete it.second;
	}
	m_Meshes.clear();
}

HRESULT Sprite3D::InitRenderer(ID3D11Device* pDevice)
{
	HRESULT hr = S_OK;

	D3D11_INPUT_ELEMENT_DESC inputLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	int numElements = ARRAYSIZE(inputLayout);


	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0x00;
	bufferDesc.StructureByteStride = 0x00;
	bufferDesc.ByteWidth = sizeof(MVP_MAT);

	hr = pDevice->CreateBuffer(&bufferDesc, nullptr, &m_VertexBuffer);
	if (FAILED(hr))
	{
		LOG(ERROR) << "Sprite3D::InitRenderer CreateBuffer Failed:" << hr;
		return E_FAIL;
	}


	Microsoft::WRL::ComPtr<ID3DBlob> pVertBlob;
	hr = Shader::CreateShaderFromFile(L"./shader/baseshader_VS.hlsl", "main", "vs_4_0", &pVertBlob);
	if (FAILED(hr))
	{
		LOG(ERROR) << "Sprite3D::InitRenderer pVertBlob D3DReadFileToBlob Failed:" << hr;
		return E_FAIL;
	}

	hr = pDevice->CreateVertexShader(pVertBlob->GetBufferPointer(), pVertBlob->GetBufferSize(), nullptr, &m_VertexShader);
	if (FAILED(hr))
	{
		LOG(ERROR) << "Sprite3D::InitRenderer CreateVertexShader Failed:" << hr << ", " << strlen(szVertShader);
		return E_FAIL;
	}

	hr = pDevice->CreateInputLayout(inputLayout, numElements, pVertBlob->GetBufferPointer(), pVertBlob->GetBufferSize(), &m_InputLayout);
	if (FAILED(hr))
	{
		LOG(ERROR) << "Sprite3D::InitRenderer CreateInputLayout Failed:" << hr;
		return E_FAIL;
	}

	Microsoft::WRL::ComPtr<ID3DBlob> pPixelBlob;
	hr = Shader::CreateShaderFromFile(L"./shader/baseshader_PS.hlsl", "main", "ps_4_0", &pPixelBlob);
	if (FAILED(hr))
	{
		LOG(ERROR) << "Sprite3D::InitRenderer pPixelBlob D3DReadFileToBlob Failed:" << hr;
		return E_FAIL;
	}

	hr = pDevice->CreatePixelShader(pPixelBlob->GetBufferPointer(), pPixelBlob->GetBufferSize(), nullptr, &m_PixelShader);
	if (FAILED(hr))
	{
		LOG(ERROR) << "Sprite3D::InitRenderer CreatePixelShader Failed:" << hr << ", " << strlen(szPixelShader);
		return E_FAIL;
	}

	return S_OK;
}
