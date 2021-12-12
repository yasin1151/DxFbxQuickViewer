#include "AssimpLoader.h"
#include "log/easylogging++.h"

AssimpLoader::AssimpLoader()
{

}

bool AssimpLoader::Load(const std::string&& szFileName, ID3D11Device* pDevice)
{
	if (!m_Meshes.empty())
	{
		for (auto* it : m_Meshes)
		{
			delete it;
		}
		m_Meshes.clear();
	}

	const aiScene* pAIScene = aiImportFile(szFileName.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

	if (!pAIScene)
	{
		LOG(ERROR) << "AssimpLoader aiImportFile Failed \n";
		return false;
	}

	const unsigned int iNumMeshes = pAIScene->mNumMeshes;
	m_Meshes.reserve(iNumMeshes);
	for (int meshIndex = 0; meshIndex < iNumMeshes; meshIndex++)
	{

		const aiMesh* pAIMesh = pAIScene->mMeshes[meshIndex];
		m_Meshes.push_back(new AssimpMesh());
		auto oAssimpMesh = m_Meshes[meshIndex];

		oAssimpMesh->Vertices.reserve(pAIMesh->mNumVertices);
		for (int vertIndex = 0; vertIndex < pAIMesh->mNumVertices; vertIndex++)
		{
			aiVector3D vert = std::move(pAIMesh->mVertices[vertIndex]);
			aiVector3D normal = std::move(pAIMesh->mNormals[vertIndex]);

			oAssimpMesh->Vertices.push_back({
					Vec3(vert.x, vert.y, vert.z),
					Vec3(normal.x, normal.y, normal.z)});
		}


		oAssimpMesh->Indices.reserve(pAIMesh->mNumFaces * 3);
		for (int faceIndex = 0; faceIndex < pAIMesh->mNumFaces; faceIndex++)
		{
			oAssimpMesh->Indices.push_back(pAIMesh->mFaces[faceIndex].mIndices[0]);
			oAssimpMesh->Indices.push_back(pAIMesh->mFaces[faceIndex].mIndices[1]);
			oAssimpMesh->Indices.push_back(pAIMesh->mFaces[faceIndex].mIndices[2]);
		}

	}

	return true;
}

std::vector<AssimpMesh*>& AssimpLoader::GetMeshes()
{
	return m_Meshes;
}
