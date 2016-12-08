#include "stdafx.h"
#include "Camera.h"

Camera::Camera() : m_yaw(0),m_pitch(0),m_roll(0)
{
	m_movementSpeed = DEFAULT_MOVEMENT_SPEED;
	m_rotationSpeed = DEFAULT_ROTATION_SPEED;
}

Camera::~Camera(void)
{
}


void Camera::CalculateLookQuaternion(D3DXQUATERNION *toCalculate)
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

void Camera::ResetMovement()
{
	m_movementOccurred = false;
	m_direction.x = 0.0f;
	m_direction.y = 0.0f;
	m_direction.z = 0.0f;
}

void Camera::CalculateNewCoords(D3DXVECTOR3* newCoords, const D3DXVECTOR3 currentCoords, const D3DXQUATERNION lookQ)
{
	newCoords->x = currentCoords.x;
	newCoords->y = currentCoords.y;
	newCoords->z = currentCoords.z;
	if (m_movementOccurred)
	{
		D3DXMATRIX lookMatrix;
		D3DXMatrixRotationQuaternion(&lookMatrix, &lookQ);
		// the lookMatrix is the transformation matrix of the look Quaternion. The first column is the x axis, the third column is the actual look vector. 
		// This is a transformation matrix so the values we need to use are negated. As Z+ is up, Y+ is out of the screen, X+ is to the right. 
		newCoords->x += lookMatrix._11 * m_direction.x;
		newCoords->y += lookMatrix._21 * m_direction.x;
		newCoords->z += lookMatrix._31 * m_direction.x;
		newCoords->x += lookMatrix._13 * -m_direction.y;
		newCoords->y += lookMatrix._23 * -m_direction.y;	// movement in the camera look direction. Y+ is out of the screen in worldspace! (Z is up!)
		newCoords->z += lookMatrix._33 * -m_direction.y;
		newCoords->z += -m_direction.z;						// Don't use the look matrix' second column (which is up) as we want Z to move along world Z axis always.
	}
}


void Camera::MoveForward(float amount) 
{ 
	m_direction.y += (m_movementSpeed * amount);			// game has Z up and Y out of the screen
	m_movementOccurred = true;
}

void Camera::MoveRight(float amount) 
{
	m_direction.x += (m_movementSpeed * amount);
	m_movementOccurred = true;
}

void Camera::MoveUp(float amount)
{ 
	m_direction.z += (m_movementSpeed * amount);
	m_movementOccurred = true;
}

void Camera::Yaw(float amount) 
{
	m_yaw+= (m_rotationSpeed * amount);
	m_yaw= ClampAngle(m_yaw);
}	

void Camera::Pitch(float amount)
{
	m_pitch+= (m_rotationSpeed * amount);
	m_pitch= ClampAngle(m_pitch);
}

void Camera::Roll(float amount) 
{
	m_roll+= (m_rotationSpeed * amount);
	m_roll= ClampAngle(m_roll);
}

void Camera::SetPitch(float angle)
{
	m_pitch = ClampAngle(angle);
}

void Camera::SetYaw(float angle)
{
	m_yaw = ClampAngle(angle);
}

void Camera::SetRoll(float angle)
{
	m_roll = ClampAngle(angle);
}

// Keep the angle in the range 0 to 360 (2*PI)
float Camera::ClampAngle(float angle) const
{
	while(angle>2*D3DX_PI)
		angle-=2*D3DX_PI;

	while(angle<0)
		angle+=2*D3DX_PI;

	return angle;
}