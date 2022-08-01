#include "pch.h"
#include "DungeonDrawer.h"

#include <Mage/Components/TilemapComponent.h>
#include <Mage/Scenegraph/GameObject.h>

#include <unordered_map>

inline int Random(int min, int max)
{
	return rand() % (max - min + 1) + min;
}

void DungeonDrawer::Initialize()
{
	m_pTilemap = GetGameObject()->GetComponent<Mage::TilemapComponent>();
}

void DungeonDrawer::DrawDungeon(const std::unordered_set<glm::ivec2>& floorTiles) const
{
	// clear previous
	m_pTilemap->EraseAll();

	enum class TileType
	{
		Floor,
		Wall,
		Roof
	};

	// tiles
	std::unordered_map<glm::ivec2, TileType> tiles{};

	// Get all floor tiles
	for (const auto& pos : floorTiles)
	{
		tiles[pos] = TileType::Floor;
	}

	// 1 above floor is wall
	for (auto& pair : tiles)
	{
		if (pair.second == TileType::Floor)
		{
			const glm::ivec2& pos = pair.first;
			if (tiles.find(pos + glm::ivec2(0, 1)) == tiles.end())
			{
				tiles[pos + glm::ivec2(0, 1)] = TileType::Wall;
			}
		}
	}

	// everything else is roof
	std::vector<glm::ivec2> roofTiles;

	for (auto& pair : tiles)
	{
		// make 8 tiles around the tile roof if empty
		const glm::ivec2& pos = pair.first;
		for (int x = -1; x <= 1; x++)
		{
			for (int y = -1; y <= 1; y++)
			{
				if (tiles.find(pos + glm::ivec2(x, y)) == tiles.end())
				{
					roofTiles.push_back(pos + glm::ivec2(x, y));
				}
			}
		}
	}

	for (const glm::ivec2& pos : roofTiles)
	{
		tiles[pos] = TileType::Roof;
	}

	// Draw to tilemap
	for (auto& pair : tiles)
	{
		const glm::ivec2& pos = pair.first;
		const TileType& type = pair.second;

		switch (type)
		{
			case TileType::Floor:
			{
				// on top and to the left not floor, outside corner
				if (tiles.at(pos + glm::ivec2(-1, 0)) != TileType::Floor &&
					tiles.at(pos + glm::ivec2(0, 1)) != TileType::Floor)
				{
					m_pTilemap->SetTile(pos, RandomTile(m_floor_shadow_outsidecorner));
				}
				// on left and not floor, and on left top floor, inside corner bottom left
				else if (tiles.at(pos + glm::ivec2(-1, 0)) != TileType::Floor &&
					tiles.at(pos + glm::ivec2(-1, 1)) == TileType::Floor)
				{
					m_pTilemap->SetTile(pos, RandomTile(m_floor_shadow_insidecorner_bottomleft));
				}
				// on top not floor, straight top
				else if (tiles.at(pos + glm::ivec2(0, 1)) != TileType::Floor)
				{
					m_pTilemap->SetTile(pos, RandomTile(m_floor_shadow_straight_top));
				}
				// on left not floor, straight left
				else if (tiles.at(pos + glm::ivec2(-1, 0)) != TileType::Floor)
				{
					m_pTilemap->SetTile(pos, RandomTile(m_floor_shadow_straight_left));
				}
				// on top left not floor, inside corner top left
				else if (tiles.at(pos + glm::ivec2(-1, 1)) != TileType::Floor)
				{
					m_pTilemap->SetTile(pos, RandomTile(m_floor_shadow_insidecorner_topleft));
				}
				// else normal floor
				else
				{
					m_pTilemap->SetTile(pos, RandomTile(m_floor));
				}

				break;
			}
			case TileType::Wall:
			{
				// floor on left, left wall
				if (tiles.at(pos + glm::ivec2(-1, 0)) == TileType::Floor)
				{
					m_pTilemap->SetTile(pos, RandomTile(m_wall_left));
				}
				// floor on right, right wall
				else if (tiles.at(pos + glm::ivec2(1, 0)) == TileType::Floor)
				{
					m_pTilemap->SetTile(pos, RandomTile(m_wall_right));
				}
				// else tile normal wall
				else
				{
					m_pTilemap->SetTile(pos, RandomTile(m_wall));
				}

				break;
			}
			case TileType::Roof:
			{
				// on left and bottom not roof, outside corner bottom left
				if (tiles.find(pos + glm::ivec2(-1, 0)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(-1, 0)) != TileType::Roof &&
					tiles.find(pos + glm::ivec2(0, -1)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(0, -1)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, RandomTile(m_roof_outsidecorner_bottomleft));
				}
				// on right and bottom not roof, outside corner bottom right
				else if (tiles.find(pos + glm::ivec2(1, 0)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(1, 0)) != TileType::Roof &&
					tiles.find(pos + glm::ivec2(0, -1)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(0, -1)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, RandomTile(m_roof_outsidecorner_bottomright));
				}
				// on left and top not roof, outside corner top left
				else if (tiles.find(pos + glm::ivec2(-1, 0)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(-1, 0)) != TileType::Roof &&
					tiles.find(pos + glm::ivec2(0, 1)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(0, 1)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, RandomTile(m_roof_outsidecorner_topleft));
				}
				// on right and top not roof, outside corner top right
				else if (tiles.find(pos + glm::ivec2(1, 0)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(1, 0)) != TileType::Roof &&
					tiles.find(pos + glm::ivec2(0, 1)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(0, 1)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, RandomTile(m_roof_outsidecorner_topright));
				}
				// on bottom not roof, straight bottom
				else if (tiles.find(pos + glm::ivec2(0, -1)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(0, -1)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, RandomTile(m_roof_straight_bottom));
				}
				// on left not roof, straight left
				else if (tiles.find(pos + glm::ivec2(-1, 0)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(-1, 0)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, RandomTile(m_roof_straight_left));
				}
				// on right not roof, straight right
				else if (tiles.find(pos + glm::ivec2(1, 0)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(1, 0)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, RandomTile(m_roof_straight_right));
				}
				// on top not roof, straight top
				else if (tiles.find(pos + glm::ivec2(0, 1)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(0, 1)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, RandomTile(m_roof_straight_top));
				}
				// on bottom left not roof, inside corner bottom left
				else if (tiles.find(pos + glm::ivec2(-1, -1)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(-1, -1)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, RandomTile(m_roof_insidecorner_bottomleft));
				}
				// on bottom right not roof, inside corner bottom right
				else if (tiles.find(pos + glm::ivec2(1, -1)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(1, -1)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, RandomTile(m_roof_insidecorner_bottomright));
				}
				// on top left not roof, inside corner top left
				else if (tiles.find(pos + glm::ivec2(-1, 1)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(-1, 1)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, RandomTile(m_roof_insidecorner_topleft));
				}
				// on top right not roof, inside corner top right
				else if (tiles.find(pos + glm::ivec2(1, 1)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(1, 1)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, RandomTile(m_roof_insidecorner_topright));
				}
				// else normal roof
				else
				{
					m_pTilemap->SetTile(pos, RandomTile(m_roof));
				}

				break;
			}
		}
	}
}

int DungeonDrawer::RandomTile(const Tile& tile) const
{
	int weightSum = 0;
	for (const auto& option : tile.options)
	{
		weightSum += option.weight;
	}

	int rnd = Random(0, weightSum - 1);
	for (const auto& option : tile.options)
	{
		if (rnd < option.weight)
			return option.tile;

		rnd -= option.weight;
	}

	assert(!"should never get here");
}
