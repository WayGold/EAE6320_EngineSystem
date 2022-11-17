#pragma once
#include "Engine/Physics/sRigidBodyState.h"
#include "Engine/Math/sVector.h"
#include <cmath>
#include <algorithm>

namespace eae6320 {
	namespace AIMath {

        const float kEpsilonNormalSqrt = static_cast<float>(exp(-15));
        const float pi = static_cast<float>(3.14159265358979323846);
        const float Rad2Deg = static_cast<float>(360 / (pi * 2));
        const float Deg2Rad = static_cast<float>((pi * 2) / 360);

        // Returns the sign of /f/.
        inline float Sign(float f) 
        { 
            return f >= 0.0f ? 1.0f : -1.0f; 
        }

        inline float sqrMagnitude(Math::sVector i_vec)
        { 
            auto x = i_vec.x;
            auto y = i_vec.y;
            auto z = i_vec.z;

            return x * x + y * y + z * z;
        }

        inline float Angle(Math::sVector from, Math::sVector to)
        {
            // sqrt(a) * sqrt(b) = sqrt(a * b) -- valid for real numbers
            float denominator = (float)sqrt(sqrMagnitude(from) * sqrMagnitude(to));
            if (denominator < kEpsilonNormalSqrt)
                return 0.0f;

            float dot = std::clamp(Dot(from, to) / denominator, -1.0f, 1.0f);
            return ((float)acos(dot)) * Rad2Deg;
        }

        // The smaller of the two possible angles between the two vectors is returned, therefore the result will never be greater than 180 degrees or smaller than -180 degrees.
        // If you imagine the from and to vectors as lines on a piece of paper, both originating from the same point, then the /axis/ vector would point up out of the paper.
        // The measured angle between the two vectors would be positive in a clockwise direction and negative in an anti-clockwise direction.
        inline float SignedAngle(Math::sVector from, Math::sVector to, Math::sVector axis)
        {
            float unsignedAngle = Angle(from, to);

            float cross_x = from.y * to.z - from.z * to.y;
            float cross_y = from.z * to.x - from.x * to.z;
            float cross_z = from.x * to.y - from.y * to.x;

            float sign = Sign(axis.x * cross_x + axis.y * cross_y + axis.z * cross_z);
            return unsignedAngle * sign;
        }

        inline float angleInRadianNegZ(Math::sVector forward) 
        {
            float angleInRadian = atan2(forward.x, forward.z);

            if (forward.x > 0) {
                angleInRadian += -AIMath::pi;
            }
            else if (forward.x < 0) {
                angleInRadian += AIMath::pi;
            }

            return angleInRadian;
        }
	}
}