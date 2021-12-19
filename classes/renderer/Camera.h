#pragma once

#include <DirectXMath.h>

class Camera
{
public:
	Camera();

	DirectX::XMMATRIX GetViewMat() const;

	DirectX::XMFLOAT3 GetCameraPos() const;

	void SetCameraPos(DirectX::XMFLOAT3 cameraPos);

	void Translate(DirectX::XMFLOAT3 deltaPos);

	DirectX::XMMATRIX GetProjectionMat() const;

	void InitProjectionMat(unsigned int screenWidth, unsigned int screenHeight, float nearZ, float farZ, float fovAngleY);


private:
	//  ”Õºæÿ’Û
	DirectX::XMFLOAT3 m_CameraPos;
	float m_Pitch;
	float m_Yaw;

	// Õ∂”∞æÿ’Û
	DirectX::XMMATRIX m_ProjMat;
};
