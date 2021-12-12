#pragma once
#include <d3d11.h>
#include <string>
#include <vector>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

#include "AssimpMesh.h"

class AssimpLoader
{
public:
	AssimpLoader();
	bool Load(const std::string&& szFileName, ID3D11Device* pDevice);

	std::vector<AssimpMesh*>& GetMeshes();

private:
	std::vector<AssimpMesh*> m_Meshes;

};