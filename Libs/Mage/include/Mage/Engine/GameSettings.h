#pragma once
#include "Mage/Singleton.h"

namespace Mage
{
	class GameSettings final : public Singleton<GameSettings>
	{
	public:
		void SetWindowSize(const glm::vec2& size) { m_WindowSize = size; }
		const glm::vec2& GetWindowSize() const { return m_WindowSize; }

		void SetWindowTitle(const std::string& title) { m_WindowTitle = title; }
		const std::string& GetWindowTitle() const { return m_WindowTitle; }
		
	private:
		friend class Singleton<GameSettings>;
		GameSettings() = default;

		glm::vec2 m_WindowSize;
		std::string m_WindowTitle;
	};
}

