#include "Camera.h"

#include "log/easylogging++.h"


Camera::Camera()
	:m_ProjMat()
{

}

DirectX::XMMATRIX Camera::GetViewMat() const
{
	using namespace DirectX;

	const XMVECTOR forwardBaseVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	const auto lookVector = XMVector3Transform(forwardBaseVector, XMMatrixRotationRollPitchYaw(m_Pitch, m_Yaw, 0.0f));

	const auto camPos = XMLoadFloat3(&m_CameraPos);
	const auto camTarget = camPos + lookVector;
	return XMMatrixLookAtLH(camPos, camTarget, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

DirectX::XMFLOAT3 Camera::GetCameraPos() const
{
	return m_CameraPos;
}

void Camera::SetCameraPos(DirectX::XMFLOAT3 cameraPos)
{
	m_CameraPos = cameraPos;
}

void Camera::Translate(DirectX::XMFLOAT3 deltaPos)
{
	if (deltaPos.x == 0 && deltaPos.y == 0 && deltaPos.z == 0)
	{
		return;
	}

	m_CameraPos = {
		m_CameraPos.x + deltaPos.x,
		m_CameraPos.y + deltaPos.y,
		m_CameraPos.z + deltaPos.z,
	};
	LOG(INFO) << "CamPos : (" << m_CameraPos.x << ", " << m_CameraPos.y << ", " << m_CameraPos.z << ")";
}


DirectX::XMMATRIX Camera::GetProjectionMat() const
{
	return m_ProjMat;
}

void Camera::InitProjectionMat(unsigned screenWidth, unsigned screenHeight, float nearZ, float farZ, float fovAngleY)
{
	m_ProjMat = DirectX::XMMatrixPerspectiveFovLH(fovAngleY, screenWidth / static_cast<float>(screenHeight), nearZ, farZ);
}
