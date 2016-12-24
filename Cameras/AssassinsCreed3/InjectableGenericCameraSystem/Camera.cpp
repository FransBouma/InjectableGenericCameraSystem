////////////////////////////////////////////////////////////////////////////////////////////////////////
// Part of Injectable Generic Camera System
// Copyright(c) 2016, Frans Bouma
// All rights reserved.
// https://github.com/FransBouma/InjectableGenericCameraSystem
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
//
//  * Redistributions of source code must retain the above copyright notice, this
//	  list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and / or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Camera.h"

using namespace DirectX;

Camera::Camera() : m_yaw(0), m_pitch(0), m_roll(0)
{
	m_movementSpeed = DEFAULT_MOVEMENT_SPEED;
	m_rotationSpeed = DEFAULT_ROTATION_SPEED;
	m_movementOccurred = false;
	m_yawDelta = 0.0f;
	m_pitchDelta = 0.0f;
	m_rollDelta = 0.0f;
}


Camera::~Camera(void)
{
}


XMVECTOR Camera::CalculateLookQuaternion()
{
	XMVECTOR xQ = XMQuaternionRotationNormal(XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f), -m_pitch);
	XMVECTOR yQ = XMQuaternionRotationNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), m_roll);		
	XMVECTOR zQ = XMQuaternionRotationNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f), -m_yaw);		

	XMVECTOR tmpQ = XMQuaternionMultiply(xQ, zQ);
	XMVECTOR qToReturn = XMQuaternionMultiply(yQ, tmpQ);
	XMQuaternionNormalize(qToReturn);
	return qToReturn;
}


void Camera::ResetDeltas()
{
	m_movementOccurred = false;
	m_direction.x = 0.0f;
	m_direction.y = 0.0f;
	m_direction.z = 0.0f;

	m_yawDelta = 0.0f;
	m_pitchDelta = 0.0f;
	m_rollDelta = 0.0f;
}


void Camera::ResetAngles()
{
	SetPitch(INITIAL_PITCH_RADIANS);
	SetRoll(INITIAL_ROLL_RADIANS);
	SetYaw(INITIAL_YAW_RADIANS);
}


XMFLOAT3 Camera::CalculateNewCoords(const XMFLOAT3 currentCoords, const XMVECTOR lookQ)
{
	XMFLOAT3 toReturn;
	toReturn.x = currentCoords.x;
	toReturn.y = currentCoords.y;
	toReturn.z = currentCoords.z;
	if (m_movementOccurred)
	{
		XMVECTOR directionAsQ = XMVectorSet(m_direction.x, m_direction.y, m_direction.z, 0.0f);
		XMVECTOR newDirection = XMVector3Rotate(directionAsQ, lookQ);
		toReturn.x += XMVectorGetX(newDirection);
		toReturn.y += XMVectorGetY(newDirection);
		toReturn.z += XMVectorGetZ(newDirection);
		toReturn.z += m_direction.z;
	}
	return toReturn;
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
	m_direction.z += (m_movementSpeed * amount * DEFAULT_Z_MOVEMENT_MULTIPLIER);
	m_movementOccurred = true;
}

void Camera::Yaw(float amount)
{
	m_yaw += (m_rotationSpeed * amount);
	m_yaw = ClampAngle(m_yaw);
	m_yawDelta += m_rotationSpeed * amount;
	m_yawDelta = ClampAngle(m_yawDelta);
}

void Camera::Pitch(float amount)
{
	m_pitch += (m_rotationSpeed * amount);
	m_pitch = ClampAngle(m_pitch);
	m_pitchDelta += m_rotationSpeed * amount;
	m_pitchDelta = ClampAngle(m_pitchDelta);
}

void Camera::Roll(float amount)
{
	m_roll += (m_rotationSpeed * amount);
	m_roll = ClampAngle(m_roll);
	m_rollDelta = m_rotationSpeed * amount;
	m_rollDelta = ClampAngle(m_rollDelta);
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
	while (angle > XM_2PI)
	{
		angle -= XM_2PI;
	}
	while (angle < 0)
	{
		angle += XM_2PI;
	}
	return angle;
}