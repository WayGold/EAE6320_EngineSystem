#pragma once
#include "../../SteeringOutput/DynamicSteering.h"

namespace eae6320 {
	namespace AIMovement {
		class DynamicArrive {
		public:
			Physics::sRigidBodyState* characterRB;
			Physics::sRigidBodyState* targetRB;

			float maxAcceleration;
			float maxSpeed;

			float targetRadius;
			float slowRadius;
			float timeToTarget;

			DynamicArrive(Physics::sRigidBodyState* i_character, Physics::sRigidBodyState* i_target, 
				float i_maxAcceleration, float i_maxSpeed, float i_targetRadius, 
				float i_slowRadius, float i_timeToTarget = 0.1) :
				characterRB(i_character), targetRB(i_target), maxAcceleration(i_maxAcceleration), maxSpeed(i_maxSpeed),
				targetRadius(i_targetRadius), slowRadius(i_slowRadius), timeToTarget(i_timeToTarget) {};

			~DynamicArrive() {};

			DynamicSteeringOutput getSteering();
		};
	}
}
