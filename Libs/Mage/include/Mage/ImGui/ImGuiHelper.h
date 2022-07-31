#pragma once
#include <functional>

struct ImFont;
struct SDL_Window;

namespace Mage
{
	struct Color;
	class Texture2D;

    class ImGuiHelper
	{
	public:
		static void InitImGui(SDL_Window* pWindow);

		static ImFont* GetFontBold();
		static ImFont* GetFontItalic();
		static ImFont* GetFontBoldItalic();

		// Widgets
		enum class ItemLabelAlignment { Left, Right };
		static void ItemLabel(const char* title, ItemLabelAlignment alignment);
		
		static bool SDLColorPicker(const char* label, Color* pColor, int imGuiColorEditFlags = 0);

		static bool ToggleButton(const char* label, bool active, const glm::vec2& size = glm::vec2(0.f, 0.f));
		static bool RadioToggleButton(const char* label, int* v, int v_button, const glm::vec2& size = glm::vec2(0.f, 0.f));

		static void Component(const char* name, void const* id, bool* enabled, const std::function<void()>& extraUIFunction);

		static void Texture(const Texture2D* texture);

	private:
		static void SetCustomStyle();
	};
}
