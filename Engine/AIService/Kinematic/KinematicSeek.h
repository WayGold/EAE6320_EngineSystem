#pragma once

#include "../SteeringOutput/KinematicSteering.h"

namespace eae6320 {
	namespace AIMovement {
		class KinematicSeek {
		public:
			Physics::sRigidBodyState* characterRB;
			Physics::sRigidBodyState* targetRB;

			float maxSpeed;

			KinematicSeek(Physics::sRigidBodyState* i_character, Physics::sRigidBodyState* i_target, float i_maxSpeed) :
				characterRB(i_character), targetRB(i_target), maxSpeed(i_maxSpeed) {};

			~KinematicSeek() {};

			KinematicSteeringOutput getSteering();
		};
	}
}