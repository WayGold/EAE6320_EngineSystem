#pragma once
#include "../../SteeringOutput/DynamicSteering.h"
#include "../Advanced/DynamicLWYG.h"
#include "DynamicSeek.h"

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

namespace eae6320 {
	namespace AIMovement {
		class DynamicWander {
		public:
			DynamicLookWhereYouAreGoing* dynamicLookWhereYouAreGoing = nullptr;
			DynamicSeek* dynamicSeek = nullptr;

			Physics::sRigidBodyState* characterRB;
			Physics::sRigidBodyState* explicitTarget = nullptr;

			float maxAngularAcceleration;
			float maxRotation;
			float maxAcceleration;

			float targetRadius;
			float slowRadius;
			float timeToTarget;
			float wanderOffset;
			float wanderRate;

			DynamicWander(Physics::sRigidBodyState* i_character, float i_maxAngularAcceleration, float i_maxRotation,
				float i_maxAcceleration, float i_targetRadius, float i_slowRadius,
				float i_wanderOffset, float i_wanderRate, float i_timeToTarget = 0.1) :
				characterRB(i_character), maxAngularAcceleration(i_maxAngularAcceleration),
				maxRotation(i_maxRotation), maxAcceleration(i_maxAcceleration), targetRadius(i_targetRadius),
				slowRadius(i_slowRadius), timeToTarget(i_timeToTarget), wanderOffset(i_wanderOffset),
				wanderRate(i_wanderRate) {};
			~DynamicWander() {};

			// Flag false -> delegate DynamicLookWhereYouAreGoing, else delegate DynamicFace
			DynamicSteeringOutput getSteering(bool flag = false);

			inline float randomBinomial() {
				/* initialize random seed: */
				srand(static_cast<unsigned int>(time(NULL)));
				return static_cast<float>(rand()) / static_cast<float>(RAND_MAX) -
					static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			}
		};
	}
}
