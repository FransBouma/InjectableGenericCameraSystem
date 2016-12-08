#pragma once
#include "stdafx.h"
#include <d3dx9.h>

class Camera
{
public:
	Camera();
	~Camera(void);

	void CalculateLookQuaternion(D3DXQUATERNION* toCalculate);
	void ResetMovement();
	void CalculateNewCoords(D3DXVECTOR3* newCoords, const D3DXVECTOR3 currentCoords, const D3DXQUATERNION lookQ);

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
	D3DXVECTOR3 m_direction; // camera direction, defined by move methods. Reset by ResetMovement
	float m_yaw;
	float m_pitch;
	float m_roll;
	bool m_movementOccurred;
	float m_movementSpeed;
	float m_rotationSpeed;

	float ClampAngle(float angle) const;
};
