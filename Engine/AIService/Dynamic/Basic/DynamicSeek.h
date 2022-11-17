#pragma once
#include "../../SteeringOutput/DynamicSteering.h"

namespace eae6320 {
	namespace AIMovement {
		class DynamicSeek {
		public:
			Physics::sRigidBodyState* characterRB;
			Physics::sRigidBodyState* targetRB;

			float maxAcceleration;

			DynamicSeek(Physics::sRigidBodyState* i_character, Physics::sRigidBodyState* i_target, 
				float i_maxAcceleration) :
				characterRB(i_character), targetRB(i_target), maxAcceleration(i_maxAcceleration) {};

			~DynamicSeek() {};

			DynamicSteeringOutput getSteering();
		};
	}
}