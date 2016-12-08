#pragma once
#include "stdafx.h"

using namespace DirectX;

class Camera
{
public:
	Camera();
	~Camera(void);

	XMVECTOR CalculateLookQuaternion();
	void ResetMovement();
	XMFLOAT3 CalculateNewCoords(const XMFLOAT3 currentCoords, const XMVECTOR lookQ);

	void MoveForward(float amount);
	void MoveRight(float amount);
	void MoveUp(float amount);
	void Yaw(float amount);
	void Pitch(float amount);
	void Roll(float amount);
	void SetPitch(float angle);
	void SetYaw(float angle);
	void SetRoll(float angle);

private:
	XMFLOAT3 m_direction; // camera direction, defined by move methods. Reset by ResetMovement
	float m_yaw;
	float m_pitch;
	float m_roll;
	bool m_movementOccurred;
	float m_movementSpeed;
	float m_rotationSpeed;

	float ClampAngle(float angle) const;
};
