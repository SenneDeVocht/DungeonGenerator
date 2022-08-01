#include "pch.h"
#include "DungeonGenerator.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <map>
#include <Mage/Components/TilemapComponent.h>
#include <Mage/Engine/ServiceLocator.h>
#include <Mage/Input/InputManager.h>
#include <Mage/Scenegraph/GameObject.h>
#include <Mage/Engine/Renderer.h>

void DungeonGenerator::Initialize()
{
	m_pTilemap = GetGameObject()->GetComponent<Mage::TilemapComponent>();
}

void DungeonGenerator::Update()
{
	const auto input = Mage::ServiceLocator::GetInputManager();

	if (input->CheckKeyboardKey('R', Mage::InputState::Down))
		GenerateDungeon();
}

void DungeonGenerator::RenderGizmos() const
{
	for (const auto& room : m_rooms)
	{
		for (const auto& rect : room.rects)
		{
			std::vector<glm::vec2> corners = {
				glm::vec2(rect.pos.x - 0.5f, rect.pos.y - 0.5f),
				glm::vec2(rect.pos.x + rect.size.x - 0.5f, rect.pos.y - 0.5f),
				glm::vec2(rect.pos.x + rect.size.x - 0.5f, rect.pos.y + rect.size.y - 0.5f),
				glm::vec2(rect.pos.x - 0.5f, rect.pos.y + rect.size.y - 0.5f)
			};

			Mage::ServiceLocator::GetRenderer()->RenderPolygonOutline(corners, true, glm::vec4(0.3f, 0.6f, 0.9f, 1.0f), 1.0f);
		}

		std::vector<glm::vec2> corners = {
				glm::vec2(room.bounds.pos.x - 0.5f, room.bounds.pos.y - 0.5f),
				glm::vec2(room.bounds.pos.x + room.bounds.size.x - 0.5f, room.bounds.pos.y - 0.5f),
				glm::vec2(room.bounds.pos.x + room.bounds.size.x - 0.5f, room.bounds.pos.y + room.bounds.size.y - 0.5f),
				glm::vec2(room.bounds.pos.x - 0.5f, room.bounds.pos.y + room.bounds.size.y - 0.5f)
		};

		Mage::ServiceLocator::GetRenderer()->RenderPolygonOutline(corners, true, glm::vec4(0.3f, 0.6f, 0.9f, 1.0f), 1.0f);
	}
}

void DungeonGenerator::GenerateDungeon()
{
	// Clear previous
	m_rooms.clear();
	m_pTilemap->EraseAll();

	// Generate Rooms
	for (int i = 0; i < m_roomAmount; i++)
	{
		Room room = GenerateRoom();
		SetRoomPosition(room);

		m_rooms.push_back(room);
	}

	// Generate Connections


	// Draw to tilemap
	DrawDungeon();
}

DungeonGenerator::Room DungeonGenerator::GenerateRoom() const
{
	Room room;

	// Root rectangle
	room.rects.push_back({
		glm::vec2(0, 0),
		glm::vec2(
			rand() % (m_maxRoomRectSize - m_minRoomRectSize) + 1 + m_minRoomRectSize,
			rand() % (m_maxRoomRectSize - m_minRoomRectSize) + 1 + m_minRoomRectSize)
	});

	// Additional rectangles
	const int amountExtraRects = rand() % (m_maxRoomRects - m_minRoomRects + 1) + m_minRoomRects - 1;
	for (int i = 0; i < amountExtraRects; i++)
	{
		Rect addRect;

		// keep generating rect until it can be added
		do
		{
			const Rect& rectToConnectTo = room.rects[rand() % room.rects.size()];

			addRect.size = glm::vec2(rand() % (m_maxRoomRectSize - m_minRoomRectSize) + 1 + m_minRoomRectSize,
				rand() % (m_maxRoomRectSize - m_minRoomRectSize) + 1 + m_minRoomRectSize);

			// set position so it shares a corner with root rect
			const int corner = rand() % 8;
			switch (corner)
			{
				// TOP
				// left
				case 0:
				{
					addRect.pos = glm::ivec2(0, rectToConnectTo.size.y);
					break;
				}
				// right
				case 1:
				{
					addRect.pos = glm::ivec2(rectToConnectTo.size.x - addRect.size.x, rectToConnectTo.size.y);
					break;
				}

				// BOTTOM
				// left
				case 2:
				{
					addRect.pos = glm::ivec2(0, -addRect.size.y);
					break;
				}
				// right
				case 3:
				{
					addRect.pos = glm::ivec2(rectToConnectTo.size.x - addRect.size.x, -addRect.size.y);
					break;
				}

				// LEFT
				// top
				case 4:
				{
					addRect.pos = glm::ivec2(-addRect.size.x, room.rects[0].size.y - addRect.size.y);
					break;
				}
				// bottom
				case 5:
				{
					addRect.pos = glm::ivec2(-addRect.size.x, 0);
					break;
				}

				// RIGHT
				// top
				case 6:
				{
					addRect.pos = glm::ivec2(rectToConnectTo.size.x, rectToConnectTo.size.y - addRect.size.y);
					break;
				}
				// bottom
				case 7:
				{
					addRect.pos = glm::ivec2(rectToConnectTo.size.x, 0);
					break;
				}
			}
			addRect.pos += rectToConnectTo.pos;
		}
		while (!CanAddRectToRoom(room, addRect));

		room.rects.push_back(addRect);
	}

	// Bounds
	room.bounds = GetRoomBounds(room);

	return room;
}

void DungeonGenerator::SetRoomPosition(Room& room)
{
	const glm::ivec2 offset = room.bounds.pos;

	if (m_rooms.empty())
		return;

	do
	{
		// choose random room to connect to
		Room& roomToConnectTo = m_rooms[rand() % m_rooms.size()];
		const Rect roomToConnectToBounds = GetRoomBounds(roomToConnectTo);

		// choose random side to connect to
		int side = rand() % 4;

		switch (side)
		{
			case 0:
			{
				// top
				room.bounds.pos = glm::ivec2(
					roomToConnectToBounds.pos.x + rand() % (roomToConnectToBounds.size.x + room.bounds.size.x - 1) - room.bounds.size.x + 1,
					roomToConnectToBounds.pos.y + roomToConnectToBounds.size.y + m_minHoleSize.y);
				break;
			}
			case 1:
			{
				// right
				room.bounds.pos = glm::ivec2(
					roomToConnectToBounds.pos.x + roomToConnectToBounds.size.x + m_minHoleSize.x,
					roomToConnectToBounds.pos.y + rand() % (roomToConnectToBounds.size.y + room.bounds.size.y - 1) - room.bounds.size.y + 1);
				break;
			}
			case 2:
			{
				// bottom
				room.bounds.pos = glm::ivec2(
					roomToConnectToBounds.pos.x + rand() % (roomToConnectToBounds.size.x + room.bounds.size.x - 1) - room.bounds.size.x + 1,
					roomToConnectToBounds.pos.y - room.bounds.size.y - m_minHoleSize.y);
				break;
			}
			case 3:
			{
				// left
				room.bounds.pos = glm::ivec2(
					roomToConnectToBounds.pos.x - room.bounds.size.x - m_minHoleSize.x,
					roomToConnectToBounds.pos.y + rand() % (roomToConnectToBounds.size.y + room.bounds.size.y - 1) - room.bounds.size.y + 1);
				break;
			}
		}

	} while (!CanAddRoomToDungeon(room.bounds));

	for (auto& rect : room.rects)
	{
		rect.pos += room.bounds.pos - offset;
	}
}

bool DungeonGenerator::CanAddRectToRoom(const Room& room, const Rect& rect) const
{
	for (const auto& r : room.rects)
	{
		const auto dist2 = DistanceBetweenRects(rect, r);

		// overlapping
		if (dist2.x < 0 && dist2.y < 0 && !m_roomRectsCanOverlap)
			return false;

		// touching
		if (dist2.x <= 0 && dist2.y <= 0)
			continue;

		// gap too small
		if (dist2.x < m_minHoleSize.x && dist2.y < m_minHoleSize.y)
			return false;
	}

	return true;
}

bool DungeonGenerator::CanAddRoomToDungeon(const Rect& bounds) const
{
	for (const auto& room : m_rooms)
	{
		const auto dist2 = DistanceBetweenRects(bounds, room.bounds);
		
		// gap too small
		if (dist2.x < m_minHoleSize.x && dist2.y < m_minHoleSize.y)
			return false;
	}

	return true;
}

void DungeonGenerator::DrawDungeon() const
{
	// TILES
	constexpr int floor = 0;
	constexpr int floor_shadow_insidecorner_bottomleft = 1;
	constexpr int floor_shadow_insidecorner_topleft = 2;
	constexpr int floor_shadow_outsidecorner = 3;
	constexpr int floor_shadow_straight_left = 4;
	constexpr int floor_shadow_straight_top = 5;
		
	constexpr int wall = 6;
	constexpr int wall_left = 7;
	constexpr int wall_right = 8;
		
	constexpr int roof = 9;
	constexpr int roof_insidecorner_bottomleft = 10;
	constexpr int roof_insidecorner_bottomright = 11;
	constexpr int roof_insidecorner_topleft = 12;
	constexpr int roof_insidecorner_topright = 13;
	constexpr int roof_outsidecorner_bottomleft = 14;
	constexpr int roof_outsidecorner_bottomright = 15;
	constexpr int roof_outsidecorner_topleft = 16;
	constexpr int roof_outsidecorner_topright = 17;
	constexpr int roof_straight_bottom = 18;
	constexpr int roof_straight_left = 19;
	constexpr int roof_straight_right = 20;
	constexpr int roof_straight_top = 21;

	enum class TileType
	{
		Floor,
		Wall,
		Roof
	};

	// tiles
	std::unordered_map<glm::ivec2, TileType> tiles{};

	// Get all floor tiles
	for (const Room& room : m_rooms)
	{
		for (const Rect& rect : room.rects)
		{
			for (int x = rect.pos.x; x < rect.pos.x + rect.size.x; x++)
			{
				for (int y = rect.pos.y; y < rect.pos.y + rect.size.y; y++)
				{
					tiles[glm::ivec2(x, y)] = TileType::Floor;
				}
			}
		}
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
					m_pTilemap->SetTile(pos, floor_shadow_outsidecorner);
				}
				// on left and not floor, and on left top floor, inside corner bottom left
				else if (tiles.at(pos + glm::ivec2(-1, 0)) != TileType::Floor &&
					tiles.at(pos + glm::ivec2(-1, 1)) == TileType::Floor)
				{
					m_pTilemap->SetTile(pos, floor_shadow_insidecorner_bottomleft);
				}
				// on top not floor, straight top
				else if (tiles.at(pos + glm::ivec2(0, 1)) != TileType::Floor)
				{
					m_pTilemap->SetTile(pos, floor_shadow_straight_top);
				}
				// on left not floor, straight left
				else if (tiles.at(pos + glm::ivec2(-1, 0)) != TileType::Floor)
				{
					m_pTilemap->SetTile(pos, floor_shadow_straight_left);
				}
				// on top left not floor, inside corner top left
				else if (tiles.at(pos + glm::ivec2(-1, 1)) != TileType::Floor)
				{
					m_pTilemap->SetTile(pos, floor_shadow_insidecorner_topleft);
				}
				// else normal floor
				else
				{
					m_pTilemap->SetTile(pos, floor);
				}

				break;
			}
			case TileType::Wall:
			{
				// floor on left, left wall
				if (tiles.at(pos + glm::ivec2(-1, 0)) == TileType::Floor)
				{
					m_pTilemap->SetTile(pos, wall_left);
				}
				// floor on right, right wall
				else if (tiles.at(pos + glm::ivec2(1, 0)) == TileType::Floor)
				{
					m_pTilemap->SetTile(pos, wall_right);
				}
				// else tile normal wall
				else
				{
					m_pTilemap->SetTile(pos, wall);
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
					m_pTilemap->SetTile(pos, roof_outsidecorner_bottomleft);
				}
				// on right and bottom not roof, outside corner bottom right
				else if (tiles.find(pos + glm::ivec2(1, 0)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(1, 0)) != TileType::Roof &&
					tiles.find(pos + glm::ivec2(0, -1)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(0, -1)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, roof_outsidecorner_bottomright);
				}
				// on left and top not roof, outside corner top left
				else if (tiles.find(pos + glm::ivec2(-1, 0)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(-1, 0)) != TileType::Roof &&
					tiles.find(pos + glm::ivec2(0, 1)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(0, 1)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, roof_outsidecorner_topleft);
				}
				// on right and top not roof, outside corner top right
				else if (tiles.find(pos + glm::ivec2(1, 0)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(1, 0)) != TileType::Roof &&
					tiles.find(pos + glm::ivec2(0, 1)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(0, 1)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, roof_outsidecorner_topright);
				}
				// on bottom not roof, straight bottom
				else if (tiles.find(pos + glm::ivec2(0, -1)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(0, -1)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, roof_straight_bottom);
				}
				// on left not roof, straight left
				else if (tiles.find(pos + glm::ivec2(-1, 0)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(-1, 0)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, roof_straight_left);
				}
				// on right not roof, straight right
				else if (tiles.find(pos + glm::ivec2(1, 0)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(1, 0)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, roof_straight_right);
				}
				// on top not roof, straight top
				else if (tiles.find(pos + glm::ivec2(0, 1)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(0, 1)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, roof_straight_top);
				}
				// on bottom left not roof, inside corner bottom left
				else if (tiles.find(pos + glm::ivec2(-1, -1)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(-1, -1)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, roof_insidecorner_bottomleft);
				}
				// on bottom right not roof, inside corner bottom right
				else if (tiles.find(pos + glm::ivec2(1, -1)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(1, -1)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, roof_insidecorner_bottomright);
				}
				// on top left not roof, inside corner top left
				else if (tiles.find(pos + glm::ivec2(-1, 1)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(-1, 1)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, roof_insidecorner_topleft);
				}
				// on top right not roof, inside corner top right
				else if (tiles.find(pos + glm::ivec2(1, 1)) != tiles.end() &&
					tiles.at(pos + glm::ivec2(1, 1)) != TileType::Roof)
				{
					m_pTilemap->SetTile(pos, roof_insidecorner_topright);
				}
				// else normal roof
				else
				{
					m_pTilemap->SetTile(pos, roof);
				}

				break;
			}
		}
	}
}

glm::ivec2 DungeonGenerator::DistanceBetweenRects(const Rect& rect1, const Rect& rect2) const
{
	// union of rects
	Rect unionRect;
	unionRect.pos = glm::ivec2(std::min(rect1.pos.x, rect2.pos.x), std::min(rect1.pos.y, rect2.pos.y));
	unionRect.size = glm::ivec2(std::max(rect1.pos.x + rect1.size.x, rect2.pos.x + rect2.size.x) - unionRect.pos.x,
		std::max(rect1.pos.y + rect1.size.y, rect2.pos.y + rect2.size.y) - unionRect.pos.y);

	return unionRect.size - rect1.size - rect2.size;
}

glm::ivec2 DungeonGenerator::DistanceBetweenRooms(const Room& room1, const Room& room2) const
{
	// set distance to max
	glm::ivec2 distance = { std::numeric_limits<int>::max(), std::numeric_limits<int>::max() };

	// find closest x and y of all distances between rects
	for (const auto& rect1 : room1.rects)
	{
		for (const auto& rect2 : room2.rects)
		{
			glm::ivec2 rectDistance = DistanceBetweenRects(rect1, rect2);
			distance.x = std::min(distance.x, rectDistance.x);
			distance.y = std::min(distance.y, rectDistance.y);
		}
	}

	return distance;
}

DungeonGenerator::Rect DungeonGenerator::GetRoomBounds(const Room& room)
{
	glm::ivec2 min = { std::numeric_limits<int>::max(), std::numeric_limits<int>::max() };
	glm::ivec2 max = { std::numeric_limits<int>::min(), std::numeric_limits<int>::min() };

	for (const auto& rect : room.rects)
	{
		min.x = std::min(min.x, rect.pos.x);
		min.y = std::min(min.y, rect.pos.y);
		max.x = std::max(max.x, rect.pos.x + rect.size.x);
		max.y = std::max(max.y, rect.pos.y + rect.size.y);
	}

	Rect bounds;
	bounds.pos = min;
	bounds.size = max - bounds.pos;
	return bounds;
}
