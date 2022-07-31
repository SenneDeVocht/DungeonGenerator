#pragma once
#include "Mage/Components/Component.h"

namespace Mage
{
	class CameraComponent : public Component
	{
	public:
		explicit CameraComponent(const glm::vec2& size)
			: m_Size{ size }
		{}

		void DrawProperties() override;

		void SetSize(const glm::vec2& size) { m_Size = size; }
		const glm::vec2& GetSize() { return m_Size; }

	private:
		glm::vec2 m_Size;
	};
}