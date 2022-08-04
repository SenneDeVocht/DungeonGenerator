#pragma once
#include <unordered_set>
#include <Mage/Components/Component.h>

namespace Mage
{
	class TilemapComponent;
}

class DungeonDrawer final : public Mage::Component
{
public:
	void Initialize() override;
	void DrawDungeon(const std::unordered_set<glm::ivec2>& floorTiles) const;

private:
	struct Tile
	{
		struct Option
		{
			int tile;
			int weight;
		};

		std::vector<Option> options;
	};

	int RandomTile(const Tile& tile) const;

	Mage::TilemapComponent* m_pTilemap;

	// TILES
	const Tile m_floor = { {{0, 10}, {1, 1}, {2, 1}} };
	const Tile m_floor_shadow_insidecorner_bottomleft = { {{3, 1}} };
	const Tile m_floor_shadow_insidecorner_topleft = {{{4, 1}}};
	const Tile m_floor_shadow_outsidecorner = {{{5, 1}}};
	const Tile m_floor_shadow_straight_left = {{{6, 1}}};
	const Tile m_floor_shadow_straight_top = {{{7, 5}, {8, 1}}};

	const Tile m_wall = {{{9, 10}, {10, 4}, {11, 0}}};
	const Tile m_wall_left = {{{12, 1}}};
	const Tile m_wall_right = {{{13, 1}}};

	const Tile m_roof = {{{14, 1}}};
	const Tile m_roof_insidecorner_bottomleft = {{{15, 1}}};
	const Tile m_roof_insidecorner_bottomright = {{{16, 1}}};
	const Tile m_roof_insidecorner_topleft = {{{17, 1}}};
	const Tile m_roof_insidecorner_topright = {{{18, 1}}};
	const Tile m_roof_outsidecorner_bottomleft = {{{19, 1}}};
	const Tile m_roof_outsidecorner_bottomright = {{{20, 1}}};
	const Tile m_roof_outsidecorner_topleft = {{{21, 1}}};
	const Tile m_roof_outsidecorner_topright = {{{22, 1}}};
	const Tile m_roof_straight_bottom = {{{23, 1}}};
	const Tile m_roof_straight_left = {{{24, 1}}};
	const Tile m_roof_straight_right = {{{25, 1}}};
	const Tile m_roof_straight_top = {{{26, 1}}};
};

