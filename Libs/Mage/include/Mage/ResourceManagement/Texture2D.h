#pragma once
#include <glew.h>

struct SDL_Surface;

namespace Mage
{
    // Simple RAII wrapper for an SDL_Texture
    class Texture2D
    {
    public:
        Texture2D(GLuint texture, int width, int height, float pixelsPerUnit, const glm::vec2& pivot = { 0.5, 0.5f });
        Texture2D(SDL_Surface* pSurface, int width, int height, float pixelsPerUnit, const glm::vec2& pivot = { 0.5, 0.5f });
        ~Texture2D();

        GLuint GetGLTexture() const { return m_Texture; }
        const glm::vec2& GetPivot() const { return m_Pivot; }
        float GetPixelsPerUnit() const { return m_PixelsPerUnit; }
        int GetWidth() const { return m_Width; }
        int GetHeight() const { return m_Height; }

    private:
        GLuint m_Texture;
        int m_Width;
        int m_Height;
        glm::vec2 m_Pivot{};
        float m_PixelsPerUnit{};
    };
}
