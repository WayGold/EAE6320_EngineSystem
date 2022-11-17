#include "DynamicLWYG.h"
#include <cmath>

eae6320::DynamicSteeringOutput eae6320::AIMovement::DynamicLookWhereYouAreGoing::getSteering()
{
	DynamicSteeringOutput result;
	Math::sVector velocity;

	// 1. Calculate the target to delegate to align
	// Check for a zero direction, and make no change if so.
	velocity = characterRB->velocity;
	if (velocity.GetLength() < 0.001) {
		result.linearAccel = Math::sVector(0, 0, 0);
		result.rotAccel = 0.0f;
		return result;
	}

	// Otherwise set the target based on the velocity.
	float angleInRadian = AIMath::angleInRadianNegZ(velocity);

	targetRB = new Physics::sRigidBodyState();
	targetRB->orientation = targetRB->orientation * Math::cQuaternion(angleInRadian, Math::sVector(0, 1, 0));

	// 2. Delegate to align.
	dynamicAlign = new DynamicAlign(characterRB, targetRB, maxAngularAcceleration,
		maxRotation, targetRadius, slowRadius);
	result = dynamicAlign->getSteering();

	delete targetRB;
	delete dynamicAlign;

	return result;
}
