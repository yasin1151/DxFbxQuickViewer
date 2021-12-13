#include "AssimpMesh.h"

#include "log/easylogging++.h"

AssimpMesh::AssimpMesh()
	: VertexBuffer(nullptr)
	, IndexBuffer(nullptr)
	, NumberOfIndices(0u)
{

}

bool AssimpMesh::InitBuffer(ID3D11Device* pDevice)
{
	HRESULT hr = {};
	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0x00;
	vbDesc.MiscFlags = 0x00;
	vbDesc.ByteWidth = sizeof(AssimpVertex) * (UINT)Vertices.size();
	vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vbDesc.StructureByteStride = 0x00;


	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = &Vertices[0];

	hr = pDevice->CreateBuffer(&vbDesc, &vertexData, &VertexBuffer);
	if (FAILED(hr))
	{
		LOG(ERROR) << "AssimpMesh::InitBuffer Failed to allocate vertex buffer for render call! " << hr << std::endl;
		return false;
	}

	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0x00;
	ibDesc.MiscFlags = 0x00;
	ibDesc.ByteWidth = sizeof(std::uint32_t) * (UINT)Indices.size();
	ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
	ibDesc.StructureByteStride = 0x00;

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = &Indices[0];

	hr = pDevice->CreateBuffer(&ibDesc, &indexData, &IndexBuffer);

	NumberOfIndices = (std::uint32_t)Indices.size();
	return true;
}
