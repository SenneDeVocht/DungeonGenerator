#pragma once

namespace Mage
{
	class BoxColliderComponent;
	class GameObject;

	class Component
	{
	public:
		virtual ~Component() = default;
		
		virtual void Initialize() {}
		virtual void Update() {}
		virtual void FixedUpdate() {}
		virtual void DrawImGui() {}
		virtual void DrawProperties();
		virtual void Render() const {}
		virtual void RenderGizmos() const {}

		virtual void OnTriggerEnter(BoxColliderComponent*) {}
		virtual void OnTriggerExit(BoxColliderComponent*) {}
		virtual void OnCollisionEnter(BoxColliderComponent*) {}
		virtual void OnCollisionExit(BoxColliderComponent*) {}

		void ChangeSceneGraph();
		virtual void OnEnable() {}
		virtual void OnDisable() {}
		virtual void OnDestroy() {}

		void SetGameObject(GameObject* pGameObject);
		GameObject* GetGameObject() const;

		virtual void Destroy();
		bool IsMarkedForDestroy() const;

		bool IsEnabled() const;
		void SetEnabled(bool enabled);
		
	protected:
		GameObject* m_pGameObject = nullptr;
		bool m_IsMarkedForDestroy = false;

		bool m_IsEnabled{ true };
		bool m_ShouldBeEnabled{ true };
	};
}
