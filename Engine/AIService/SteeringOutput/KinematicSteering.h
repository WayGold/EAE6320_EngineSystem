#pragma once

#include "Engine/Physics/sRigidBodyState.h"
#include "Engine/Math/sVector.h"

namespace eae6320 {
	struct KinematicSteeringOutput {
		Math::sVector linearVelocity;
		float rotVelocity = 0.0f;

		inline KinematicSteeringOutput operator+=(const KinematicSteeringOutput& rhs) {
			linearVelocity += rhs.linearVelocity;
			rotVelocity += rhs.rotVelocity;
			return *this;
		}
	};
}

