#include "DynamicAlign.h"

eae6320::DynamicSteeringOutput eae6320::AIMovement::DynamicAlign::getSteering()
{
	DynamicSteeringOutput result;
	float rotation, rotationSize, targetRotation, angularAcceleration;

	Math::sVector targetForward = targetRB->orientation.CalculateForwardDirection();
	Math::sVector characterForward = characterRB->orientation.CalculateForwardDirection();

	// Nullify Y Axis Movement
	targetForward.y = 0.0f;
	characterForward.y = 0.0f;

	// Get the naive direction to the target.
	rotation = AIMath::SignedAngle(characterForward, targetForward, Math::sVector(0, 1, 0));
	rotation = AIMath::Deg2Rad * rotation;

	// Map the result to the (-pi, pi) interval.
	rotation = mapToRange(rotation);
	rotationSize = std::abs(rotation);

	// Check if we are there, return no steering.
	if (rotationSize < targetRadius) {
		result.linearAccel = Math::sVector(0, 0, 0);
		result.rotAccel = 0;
		return result;
	}

	// If we are outside the slowRadius, then use maximum rotation.
	if (rotationSize > slowRadius) {
		targetRotation = maxRotation;
	}
	// Otherwise calculate a scaled rotation.
	else {
		targetRotation = maxRotation * rotationSize / slowRadius;
	}

	// The final target rotation combines speed (already in the variable) and direction.
	targetRotation *= rotation / rotationSize;

	// Acceleration tries to get to the target rotation.
	result.rotAccel = (targetRotation - characterRB->angularSpeed) / timeToTarget;

	// Check if the acceleration is too great. Clamp to maxAngularAccel if too great.
	angularAcceleration = std::abs(result.rotAccel);
	if (angularAcceleration > maxAngularAcceleration) {
		result.rotAccel /= angularAcceleration;
		result.rotAccel *= maxAngularAcceleration;
	}

	result.linearAccel = Math::sVector(0, 0, 0);

	return result;
}

float eae6320::AIMovement::DynamicAlign::mapToRange(float i_rotation)
{
	while (i_rotation > AIMath::pi) {
		i_rotation -= 2 * AIMath::pi;
	}

	while (i_rotation < -AIMath::pi) {
		i_rotation += 2 * AIMath::pi;
	}

	return i_rotation;
}
