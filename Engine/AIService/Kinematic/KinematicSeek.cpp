#include "KinematicSeek.h"

eae6320::KinematicSteeringOutput eae6320::AIMovement::KinematicSeek::getSteering()
{
	KinematicSteeringOutput result;

	// Get the direction to the target.
	result.linearVelocity = targetRB->position - characterRB->position;

	// Give full acceleration along this direction.
	result.linearVelocity = result.linearVelocity.GetNormalized();
	result.linearVelocity *= maxSpeed;

	result.rotVelocity = 0;

	return result;
}
