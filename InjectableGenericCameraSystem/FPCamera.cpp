#include "stdafx.h"
#include "FPCamera.h"

CFPCamera::CFPCamera(D3DXVECTOR3 startPos) : m_position(startPos),m_yaw(0),m_pitch(0),m_roll(0)
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
	D3DXQuaternionRotationAxis(&yQ, &axis, m_roll);		// game has Z up and Y inwards, so roll is used for Y rotation
	axis = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXQuaternionRotationAxis(&zQ, &axis, m_yaw);		// game has Z up and Y inwards, so yaw is used for Z rotation

	// order is important! rotation Z * rotation Y * rotation X. Unconventional because world axis don't align with camera (Z is up, Y is into the screen)
	D3DXQuaternionMultiply(&tmpQ, &zQ, &yQ);
	D3DXQuaternionMultiply(toCalculate, &tmpQ, &xQ);
}


void CFPCamera::CalculateCameraCoords(D3DXVECTOR3* toCalculate, D3DXQUATERNION lookQ, D3DXVECTOR3 currentCoords)
{
	// TODO! 
}



// Yaw - rotation around y axis
void CFPCamera::Yaw(float amount) 
{
	m_yaw+=amount;
	m_yaw=RestrictAngleTo360Range(m_yaw);
}	

// Pitch - rotation around x axis
void CFPCamera::Pitch(float amount)
{
	m_pitch+=amount;
	m_pitch=RestrictAngleTo360Range(m_pitch);
}

// Roll - rotation around z axis
// Note: normally only used for aircraft type cameras rather than land based ones
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