#pragma once
#include "Mage/Components/Component.h"

#include "Mage/structs.h"

namespace Mage
{
	struct Color;
	class Font;
	class Texture2D;

	class TextComponent : public Component
	{
	public:
		enum class TextAlignment
		{
			Left,
			Center,
			Right
		};

		explicit TextComponent(const std::string& text, const std::shared_ptr<Font>& font, const Color& color, float pixelsPerUnit,
			const glm::vec2& pivot = { 0.0f, 0.0f }, TextAlignment alignment = TextAlignment::Left, int lineSpacing = 0, float layer = 0.0f);

		void Update() override;
		void Render() const override;
		void DrawProperties() override;

		void SetText(const std::string& text);
		void SetColor(const Color& color);

	private:
		std::unique_ptr<Texture2D> m_pTexture;
		float m_Layer;

		bool m_NeedsUpdate{ true };
		std::shared_ptr<Font> m_Font;
		std::string m_Text;
		Color m_Color;
		TextAlignment m_Alignment;
		int m_LineSpacing;
		float m_PixelsPerUnit;
		glm::vec2 m_Pivot;
	};
}