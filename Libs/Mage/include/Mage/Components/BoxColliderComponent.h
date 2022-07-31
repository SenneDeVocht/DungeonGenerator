#pragma once
#include "Mage/Components/Component.h"

class b2Fixture;

namespace Mage
{
    class RigidBodyComponent;

    class BoxColliderComponent final : public Component
	{
	public:
		BoxColliderComponent(const glm::vec2& size, const glm::vec2& offset, float angle, bool isTrigger = false);

		void Initialize() override;
		void RenderGizmos() const override;
		void DrawProperties() override;
		void OnEnable() override;
		void OnDisable() override;
		void OnDestroy() override;
		void Update() override;

		const glm::vec2& GetSize() const;
		void SetSize(const glm::vec2& size);

		const glm::vec2& GetOffset() const;
		void SetOffset(const glm::vec2& offset);

		float GetRotation() const;
		void SetRotation(float angle);

		bool IsTrigger() const;
		void SetTrigger(bool isTrigger);

	private:
		friend class PhysicsHandler;
		friend class RigidBodyComponent;
		friend class Transform;

		b2Fixture* GetRunTimeFixture() const { return m_RunTimeFixture; }
		void SetRunTimeFixture(b2Fixture* fixture) { m_RunTimeFixture = fixture; }

		void RigidBodyChanged();

		void RecalculateShape();

		void NotifyGameObjectOnTriggerEnter(BoxColliderComponent* other) const;
		void NotifyGameObjectOnTriggerExit(BoxColliderComponent* other) const;
		void NotifyGameObjectOnCollisionEnter(BoxColliderComponent* other) const;
		void NotifyGameObjectOnCollisionExit(BoxColliderComponent* other) const;

		void AttachToRigidbody(const GameObject* gameObject);

		// Collider shape
		glm::vec2 m_Size = { 1.f, 1.f };
		glm::vec2 m_Offset = { 0.f, 0.f };
		float m_Rotation = 0.f;

		bool m_IsTrigger = false;

		RigidBodyComponent* m_pRigidbody = nullptr;
		bool m_RigidbodyChanged = false;

		// storage for runtime
		b2Fixture* m_RunTimeFixture = nullptr;
	};
}
