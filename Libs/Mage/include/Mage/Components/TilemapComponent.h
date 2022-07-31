#pragma once
#include "Component.h"

#include <unordered_map>

namespace Mage
{
    class Texture2D;

    class TilemapComponent : public Component
    {
    public:
        TilemapComponent(const std::vector<std::shared_ptr<Texture2D>>& tileTextures, const glm::vec2& tileSpacing = {1, 1});
        ~TilemapComponent() = default;

        TilemapComponent(const TilemapComponent& other) = delete;
        TilemapComponent(TilemapComponent&& other) noexcept = delete;
        TilemapComponent& operator=(const TilemapComponent& other) = delete;
        TilemapComponent& operator=(TilemapComponent&& other) noexcept = delete;

        void Render() const override;
        void DrawProperties() override;

        void SetTile(const glm::ivec2& position, int tileIndex);
        void EraseTile(const glm::ivec2& position);
        void EraseAll();

    private:
        std::vector<std::shared_ptr<Texture2D>> m_TileTextures;
        glm::vec2 m_TileSpacing;

        std::unordered_map<glm::ivec2, int> m_Tilemap;
    };
}
