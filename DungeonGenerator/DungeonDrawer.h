#pragma once
#include <unordered_set>
#include <Mage/Components/Component.h>

class DungeonGenerator;

namespace Mage
{
	class TilemapComponent;
}

class DungeonDrawer final : public Mage::Component
{
public:
	void Initialize() override;
	void DrawDungeon(const DungeonGenerator* dungeonGenerator) const;

private:
	struct Tile
	{
		struct Option
		{
			int index;
			int weight;
		};

		std::vector<Option> options;

		Tile(const std::vector<Option>& _options)
			: options(_options)
		{}

		Tile(int index)
			: options({{index, 1}})
		{}
	};

	int RandomTile(const Tile& tile) const;

	Mage::TilemapComponent* m_pTilemap{};

	// TILES
	const Tile m_floor = { {{0, 10}, {1, 1}, {2, 1}} };
	const Tile m_floor_shadow_insidecorner_bottomleft = { 3 };
	const Tile m_floor_shadow_insidecorner_topleft = { 4 };
	const Tile m_floor_shadow_outsidecorner = { 5 };
	const Tile m_floor_shadow_straight_left = { 6 };
	const Tile m_floor_shadow_straight_top = {{{7, 5}, {8, 1}}};

	const Tile m_wall = {{{9, 10}, {10, 4}, {11, 0}}};
	const Tile m_wall_left = { 12 };
	const Tile m_wall_right = { 13 };

	const Tile m_roof = { 14 };
	const Tile m_roof_insidecorner_bottomleft = { 15 };
	const Tile m_roof_insidecorner_bottomright = { 16 };
	const Tile m_roof_insidecorner_topleft = { 17 };
	const Tile m_roof_insidecorner_topright = { 18 };
	const Tile m_roof_outsidecorner_bottomleft = { 19 };
	const Tile m_roof_outsidecorner_bottomright = { 20 };
	const Tile m_roof_outsidecorner_topleft = { 21 };
	const Tile m_roof_outsidecorner_topright = { 22 };
	const Tile m_roof_straight_bottom = { 23 };
	const Tile m_roof_straight_left = { 24 };
	const Tile m_roof_straight_right = { 25 };
	const Tile m_roof_straight_top = { 26 };

	const Tile m_exit = { 27 };
};

