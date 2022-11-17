#pragma once

#include "Kinematic/KinematicSeek.h"
#include "Dynamic/Basic/DynamicSeek.h"
#include "Dynamic/Basic/DynamicArrive.h"
#include "Dynamic/Advanced/DynamicLWYG.h"
#include "Dynamic/Basic/DynamicWander.h"

namespace eae6320 {
	namespace AIUpdate {
		inline void updateRigidBody(DynamicSteeringOutput i_steering, eae6320::Physics::sRigidBodyState* i_rigidbody,
			float i_maxSpeed, float i_maxRotation, float i_time) {

			// Update Rigidbody Velocity and Angular Velocity
			i_rigidbody->velocity += i_steering.linearAccel * i_time;
			i_rigidbody->angularSpeed += i_steering.rotAccel * i_time;

			// Check for speeding and clip.
			if (i_rigidbody->velocity.GetLength() > i_maxSpeed) {
				i_rigidbody->velocity = i_rigidbody->velocity.GetNormalized();
				i_rigidbody->velocity *= i_maxSpeed;
			}

			// Check for rot speeding and clip.
			if (i_rigidbody->angularSpeed > i_maxRotation) {
				i_rigidbody->angularSpeed = i_maxRotation;
			}
		}
	}
}