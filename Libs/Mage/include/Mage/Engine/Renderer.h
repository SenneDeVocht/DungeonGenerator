#pragma once

struct SDL_Window;

namespace Mage
{
	struct Color;
	class CameraComponent;
	class Texture2D;

	class Renderer
	{
	public:
		virtual ~Renderer() = default;
		
		virtual void Render() = 0;

		virtual void SetCamera(CameraComponent* pCamera) = 0;
		virtual void SetBackgroundColor(const Color& color) = 0;

		virtual void RenderPolygonOutline(const std::vector<glm::vec2>& positions, bool closed, const glm::vec4& color, float layer = 0) = 0;
		virtual void RenderPolygonFilled(const std::vector<glm::vec2>& positions, const glm::vec4& color, float layer = 0) = 0;
		virtual void RenderTexture(const Texture2D* texture, const glm::vec2& position, float rotation, const glm::vec2& scale, float layer = 0) = 0;
		virtual void RenderPartialTexture(const Texture2D* texture, int srcX, int srcY, int srcW, int srcH, const glm::vec2& position, float rotation, const glm::vec2& scale, float layer = 0) = 0;
	};

	class NullRenderer final : public Renderer
	{
	public:
		void Render() override {}

		void SetCamera(CameraComponent*) override {}
		void SetBackgroundColor(const Color&) override {}

		void RenderPolygonOutline(const std::vector<glm::vec2>&, bool, const glm::vec4&, float) override {}
		void RenderPolygonFilled(const std::vector<glm::vec2>&, const glm::vec4&, float) override {}
		void RenderTexture(const Texture2D*, const glm::vec2&, float, const glm::vec2&, float) override {}
		void RenderPartialTexture(const Texture2D*, int, int, int, int, const glm::vec2&, float, const glm::vec2&, float) override {}
	};

	class GLRenderer final : public Renderer
	{
	public:
		GLRenderer(SDL_Window* window);
		~GLRenderer() override;

        GLRenderer(const GLRenderer& other) = delete;
        GLRenderer(GLRenderer&& other) noexcept = delete;
        GLRenderer& operator=(const GLRenderer& other) = delete;
        GLRenderer& operator=(GLRenderer&& other) noexcept = delete;

		void Render() override;
		
		void SetCamera(CameraComponent* pCamera) override;
		void SetBackgroundColor(const Color& color) override;

		void RenderPolygonOutline(const std::vector<glm::vec2>& positions, bool closed, const glm::vec4& color, float layer) override;
		void RenderPolygonFilled(const std::vector<glm::vec2>& positions, const glm::vec4& color, float layer = 0) override;
		void RenderTexture(const Texture2D* texture, const glm::vec2& position, float rotation, const glm::vec2& scale, float layer = 0) override;
		void RenderPartialTexture(const Texture2D* texture, int srcX, int srcY, int srcW, int srcH, const glm::vec2& position, float rotation, const glm::vec2& scale, float layer = 0) override;

	private:
		class GLRendererImpl;
		std::unique_ptr<GLRendererImpl> m_pImpl;
	};
}
