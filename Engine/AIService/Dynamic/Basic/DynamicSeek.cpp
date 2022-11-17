#include "DynamicSeek.h"

eae6320::DynamicSteeringOutput eae6320::AIMovement::DynamicSeek::getSteering()
{
	DynamicSteeringOutput result;

	// Get the direction to the target.
	result.linearAccel = targetRB->position - characterRB->position;

	// Give full acceleration along this direction.
	result.linearAccel = result.linearAccel.GetNormalized();
	result.linearAccel *= maxAcceleration;

	result.rotAccel = 0;

	return result;
}