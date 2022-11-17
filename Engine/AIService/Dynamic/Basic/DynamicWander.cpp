#include "DynamicWander.h"

eae6320::DynamicSteeringOutput eae6320::AIMovement::DynamicWander::getSteering(bool flag)
{
	DynamicSteeringOutput result;
	float targetOrient, charOrient;
	Math::sVector direction, targetDirection;

	// Calc character orientation unit vector with orientation
	direction.x = characterRB->orientation.CalculateForwardDirection().x;
	direction.z = characterRB->orientation.CalculateForwardDirection().z;
	direction.y = 0;
	direction = direction.GetNormalized();

	// Calc target orientation and orientation unit vector
	charOrient = AIMath::angleInRadianNegZ(direction);
	targetOrient = abs(wanderRate - charOrient) * randomBinomial();
	targetDirection.z = cosf(targetOrient);
	targetDirection.x = sinf(targetOrient);
	targetDirection = targetDirection.GetNormalized();

	// Put up the wander target
	explicitTarget = new Physics::sRigidBodyState();
	explicitTarget->orientation = explicitTarget->orientation * Math::cQuaternion(targetOrient, Math::sVector(0, 1, 0));;
	explicitTarget->position = direction * wanderOffset + targetDirection + characterRB->position;

	// delegate to seek
	dynamicSeek = new DynamicSeek(characterRB, explicitTarget, maxAcceleration);
	result.linearAccel = dynamicSeek->getSteering().linearAccel;

	// delegate to LookWhereYouAreGoing
	dynamicLookWhereYouAreGoing = new DynamicLookWhereYouAreGoing(characterRB, maxAngularAcceleration, maxRotation, targetRadius, slowRadius);
	result.rotAccel = dynamicLookWhereYouAreGoing->getSteering().rotAccel;
	
	if (explicitTarget)
		delete explicitTarget;
	if (dynamicSeek)
		delete dynamicSeek;
	if (dynamicLookWhereYouAreGoing)
		delete dynamicLookWhereYouAreGoing;

	return result;
}