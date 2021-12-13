#pragma once

#include <d3d11.h>
#include <vector>
#include <wrl/client.h>

#include "Vec3.h"


class AssimpMesh
{
public:
	AssimpMesh();

	struct AssimpVertex
	{
		Vec3 Position;
		float __a;		// 保持对其
		Vec3 Normal;
		float __b;

		AssimpVertex()
			: Position(0.0f, 0.0f, 0.0f), __a(0),
			  Normal(0.0f, 0.0f, 0.0f), __b(0)
		{
		}

		AssimpVertex(const Vec3& pos, const Vec3& normal)
			: Position(pos), __a(0), Normal(normal), __b(0)
		{
		}
	};

	std::vector<AssimpVertex> Vertices;
	std::vector<std::uint32_t> Indices;


	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;
	std::uint32_t NumberOfIndices;


	bool InitBuffer(ID3D11Device* pDevice);
};



