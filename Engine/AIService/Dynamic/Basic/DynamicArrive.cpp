#include "DynamicArrive.h"

eae6320::DynamicSteeringOutput eae6320::AIMovement::DynamicArrive::getSteering()
{
	DynamicSteeringOutput result;
	Math::sVector targetVelocity;
	float targetSpeed;

	// Get the direction to the target.
	Math::sVector direction = targetRB->position - characterRB->position;
	float distance = direction.GetLength();

	// Check if we are there, return no steering.
	if (distance < targetRadius) {
		result.linearAccel = Math::sVector(0, 0, 0);
		result.rotAccel = 0;
		return result;
	}

	// If we are outside the slowRadius, then move at max speed.
	if (distance > slowRadius) {
		targetSpeed = maxSpeed;
	}
	// Otherwise calculate a scaled speed.
	else {
		targetSpeed = maxSpeed * distance / slowRadius;
	}

	// The target velocity combines speed and direction.
	targetVelocity = direction.GetNormalized();
	targetVelocity *= targetSpeed;

	// Acceleration tries to get to the target velocity.
	result.linearAccel = targetVelocity - characterRB->velocity;
	result.linearAccel /= timeToTarget;

	// Check if the acceleration is too fast.
	if (result.linearAccel.GetLength() > maxAcceleration) {
		result.linearAccel = (result.linearAccel.GetNormalized()) * maxAcceleration;
	}

	result.rotAccel = 0;
	return result;
}