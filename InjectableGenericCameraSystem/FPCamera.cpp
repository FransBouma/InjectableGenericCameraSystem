#include "stdafx.h"
#include "FPCamera.h"

CFPCamera::CFPCamera() : m_yaw(0),m_pitch(0),m_roll(0)
{
}

CFPCamera::~CFPCamera(void)
{
}


void CFPCamera::CalculateLookQuaternion(D3DXQUATERNION *toCalculate)
{
	D3DXQUATERNION xQ, yQ, zQ, tmpQ;
	D3DXVECTOR3 axis = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	D3DXQuaternionRotationAxis(&xQ, &axis, m_pitch);
	axis = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXQuaternionRotationAxis(&yQ, &axis, m_roll);		// game has Z up and Y+ out of the screen, so roll is used for Y rotation
	axis = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXQuaternionRotationAxis(&zQ, &axis, m_yaw);		// game has Z up and Y+ out of the screen, so yaw is used for Z rotation

	// order is important! rotation Z * rotation Y * rotation X. Unconventional because world axis don't align with camera (Z is up, Y+ is out of the screen)
	D3DXQuaternionMultiply(&tmpQ, &zQ, &yQ);
	D3DXQuaternionMultiply(toCalculate, &tmpQ, &xQ);
	D3DXQuaternionNormalize(toCalculate, toCalculate);
}

void CFPCamera::ResetMovement()
{
	m_movementOccurred = false;
	m_direction.x = 0.0f;
	m_direction.y = 0.0f;
	m_direction.z = 0.0f;
}

void CFPCamera::CalculateNewCoords(D3DXVECTOR3* newCoords, const D3DXVECTOR3 currentCoords, const D3DXQUATERNION lookQ)
{
	newCoords->x = currentCoords.x;
	newCoords->y = currentCoords.y;
	newCoords->z = currentCoords.z;
	if (m_movementOccurred)
	{
		// first normalize the direction vector with the deltas for the movement as the vector isn't of length 1
		D3DXVec3Normalize(&m_direction, &m_direction);
		D3DXMATRIX lookMatrix;
		D3DXMatrixRotationQuaternion(&lookMatrix, &lookQ);
		// the lookMatrix is the transformation matrix of the look Quaternion. The first column is the x axis, the third column is the actual look vector. 
		// This is a transformation matrix so the values we need to use are negated. As Z+ is up, Y+ is our of the screen, X+ is to the right. 
		newCoords->x += lookMatrix._11 * m_direction.x;
		newCoords->y += lookMatrix._21 * m_direction.x;
		newCoords->z += lookMatrix._31 * m_direction.x;
		newCoords->x += lookMatrix._13 * -m_direction.y;
		newCoords->y += lookMatrix._23 * -m_direction.y;	// movement in the camera look direction. Y+ is out of the screen in worldspace! (Z is up!)
		newCoords->z += lookMatrix._33 * -m_direction.y;
		newCoords->z += -m_direction.z;						// Don't use the look matrix' second column (which is up) as we want Z to move along world Z axis always.
	}
}


void CFPCamera::MoveForward(float amount) 
{ 
	m_direction.y += amount;							// game has Z up and Y out of the screen
	m_movementOccurred = true;
}

void CFPCamera::MoveRight(float amount) 
{
	m_direction.x += amount;
	m_movementOccurred = true;
}

void CFPCamera::MoveUp(float amount)
{ 
	m_direction.z += amount;
	m_movementOccurred = true;
}


void CFPCamera::Yaw(float amount) 
{
	m_yaw+=amount;
	m_yaw=RestrictAngleTo360Range(m_yaw);
}	

void CFPCamera::Pitch(float amount)
{
	m_pitch+=amount;
	m_pitch=RestrictAngleTo360Range(m_pitch);
}

void CFPCamera::Roll(float amount) 
{
	m_roll+=amount;
	m_roll=RestrictAngleTo360Range(m_roll);
}

// Keep the angle in the range 0 to 360 (2*PI)
float CFPCamera::RestrictAngleTo360Range(float angle) const
{
	while(angle>2*D3DX_PI)
		angle-=2*D3DX_PI;

	while(angle<0)
		angle+=2*D3DX_PI;

	return angle;
}