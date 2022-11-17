#pragma once

#include "Engine/Physics/sRigidBodyState.h"
#include "Engine/Math/sVector.h"
#include <cmath>

namespace eae6320 {
	struct DynamicSteeringOutput {
		Math::sVector linearAccel;
		float rotAccel = 0.0f;

		inline DynamicSteeringOutput operator+=(const DynamicSteeringOutput& rhs) {
			linearAccel += rhs.linearAccel;
			rotAccel += rhs.rotAccel;
			return *this;
		}
	};
}