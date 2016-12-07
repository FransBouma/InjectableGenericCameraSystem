#pragma once
#include "stdafx.h"
#include <d3dx9.h>

class CFPCamera
{
public:
	CFPCamera();
	~CFPCamera(void);

	void CalculateLookQuaternion(D3DXQUATERNION* toCalculate);
	void ResetMovement();
	void CalculateNewCoords(D3DXVECTOR3* newCoords, const D3DXVECTOR3 currentCoords, const D3DXQUATERNION lookQ);
	float GetYaw() const {return m_yaw;}
	float GetPitch() const {return m_pitch;}
	float GetRoll() const {return m_roll;}

	// Move operations
	void MoveForward(float amount);
	void MoveRight(float amount);
	void MoveUp(float amount);

	// Rotations
	void Yaw(float amount);
	void Pitch(float amount);
	void Roll(float amount);

private:
	D3DXVECTOR3 m_direction; // camera direction, defined by move methods. Reset by ResetMovement
	float m_yaw;
	float m_pitch;
	float m_roll;
	bool m_movementOccurred;

	float RestrictAngleTo360Range(float angle) const;
};
