#pragma once
#include "Mesh.h"
#include "Effect.h"
#include "Engine/Physics/sRigidBodyState.h"

namespace eae6320 {
	namespace RenderObject {
		class RenderObject {
		public:
			Graphics::Mesh::Mesh* m_mesh;
			Graphics::Effect::Effect* m_effect = nullptr;
			Physics::sRigidBodyState m_rigidBody;

			RenderObject(Graphics::Mesh::Mesh* i_mesh, Graphics::Effect::Effect* i_effect, Physics::sRigidBodyState i_rigidBody) :
				m_mesh(i_mesh), m_effect(i_effect), m_rigidBody(i_rigidBody) {
				if (m_mesh)
					m_mesh->IncrementReferenceCount();
				if (m_effect)
					m_effect->IncrementReferenceCount();
			};

			~RenderObject() {
				if (m_mesh)
					m_mesh->DecrementReferenceCount();
				if (m_effect)
					m_effect->DecrementReferenceCount();
			};

			void BindEffect(Graphics::Effect::Effect* i_effect) {
				if (m_effect) {
					m_effect->DecrementReferenceCount();
					m_effect = i_effect;
					m_effect->IncrementReferenceCount();
				}
				else {
					m_effect = i_effect;
					m_effect->IncrementReferenceCount();
				}
			};
		};
	}
}