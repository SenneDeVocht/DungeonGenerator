#include "pch.h"
#include "DungeonGenerator.h"

#include <chrono>

#include <Mage/Components/TilemapComponent.h>
#include <Mage/Engine/ServiceLocator.h>
#include <Mage/Input/InputManager.h>
#include <Mage/Scenegraph/GameObject.h>
#include <Mage/Engine/Renderer.h>

#include "DungeonDrawer.h"

inline int Random(int min, int max)
{
	return rand() % (max - min + 1) + min;
}

void DungeonGenerator::RenderGizmos() const
{
	const auto renderer = Mage::ServiceLocator::GetRenderer();

	for (const auto& room : m_rooms)
	{
		// Component Rects
		for (const auto& rect : room.rects)
		{
			std::vector<glm::vec2> corners = {
				glm::vec2(rect.pos.x - 0.5f, rect.pos.y - 0.5f),
				glm::vec2(rect.pos.x + rect.size.x - 0.5f, rect.pos.y - 0.5f),
				glm::vec2(rect.pos.x + rect.size.x - 0.5f, rect.pos.y + rect.size.y - 0.5f),
				glm::vec2(rect.pos.x - 0.5f, rect.pos.y + rect.size.y - 0.5f)
			};

			renderer->RenderPolygonOutline(corners, true, glm::vec4(0.f, 1.f, 1.f, 1.0f), 1.f);
			renderer->RenderPolygonFilled(corners, glm::vec4(0.f, 1.f, 1.f, .50f), 1.f);
		}

		// Bounds
		std::vector<glm::vec2> corners = {
				glm::vec2(room.bounds.pos.x - 0.5f, room.bounds.pos.y - 0.5f),
				glm::vec2(room.bounds.pos.x + room.bounds.size.x - 0.5f, room.bounds.pos.y - 0.5f),
				glm::vec2(room.bounds.pos.x + room.bounds.size.x - 0.5f, room.bounds.pos.y + room.bounds.size.y - 0.5f),
				glm::vec2(room.bounds.pos.x - 0.5f, room.bounds.pos.y + room.bounds.size.y - 0.5f)
		};

		renderer->RenderPolygonOutline(corners, true, glm::vec4(1.f, 0.f, 0.f, 1.0f), 1.f);
		
		// Connection to parent
		if (room.parent != -1)
		{
			std::vector<glm::vec2> points = {
				glm::vec2(room.bounds.pos) + glm::vec2(room.bounds.size) / 2.0f - 0.5f,
				glm::vec2(m_rooms[room.parent].bounds.pos) + glm::vec2(m_rooms[room.parent].bounds.size) / 2.0f - 0.5f
			};

			renderer->RenderPolygonOutline(points, false, glm::vec4(0.f, 1.f, 0.f, 1.f), 1.f);
		}
	}
}

void DungeonGenerator::GenerateDungeon()
{
	const auto seed = rand();
	srand(seed);
	std::cout << "Seed: " << seed << std::endl;


	const auto start = std::chrono::high_resolution_clock::now();

	// Clear previous
	ClearDungeon();

	// Generate Rooms
	for (int i = 0; i < m_roomAmount; i++)
	{
		GenerateRoom();
	}

	// Generate Connections
	for (const auto& room : m_rooms)
	{
		GenerateConnectionToParent(room);
	}

	// Draw to tilemap
	GetGameObject()->GetComponent<DungeonDrawer>()->DrawDungeon(m_floorTiles);

	const auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Generation time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
	std::cout << std::endl;
}

void DungeonGenerator::ClearDungeon()
{
	m_rooms.clear();
	m_floorTiles.clear();
}

void DungeonGenerator::GenerateRoom()
{
	Room room;

	// Root rectangle
	room.rects.push_back({
		glm::vec2(0, 0),
		glm::vec2(
			Random(m_minRoomRectSize, m_maxRoomRectSize),
			Random(m_minRoomRectSize, m_maxRoomRectSize))
	});

	// Additional rectangles
	const int amountExtraRects = Random(m_minRoomRects, m_maxRoomRects) - 1;
	for (int i = 0; i < amountExtraRects; i++)
	{
		Rect addRect;

		// keep generating rect until it can be added
		do
		{
			const Rect& rectToConnectTo = room.rects[Random(0, room.rects.size() - 1)];

			addRect.size = glm::vec2(
				Random(m_minRoomRectSize, m_maxRoomRectSize),
				Random(m_minRoomRectSize, m_maxRoomRectSize));

			// set position so it shares a corner with root rect
			const int corner = Random(0, 7);
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

	// Position
	SetRoomPosition(room);

	// Add floor tiles
	for (const auto& rect : room.rects)
	{
		for (int x = rect.pos.x; x < rect.pos.x + rect.size.x; x++)
		{
			for (int y = rect.pos.y; y < rect.pos.y + rect.size.y; y++)
			{
				m_floorTiles.emplace(glm::ivec2(x, y));
			}
		}
	}

	m_rooms.push_back(room);
}

void DungeonGenerator::SetRoomPosition(Room& room) const
{
	const glm::ivec2 offset = room.bounds.pos;

	if (m_rooms.empty())
		return;
	
	do
	{
		// choose random room to connect to
		const int roomToConnectTo = Random(0, m_rooms.size() - 1);
		const Rect roomToConnectToBounds = GetRoomBounds(m_rooms[roomToConnectTo]);
		room.parent = roomToConnectTo;

		// choose random side to connect to
		int side = Random(0, 3);

		switch (side)
		{
			case 0:
			{
				// top
				room.bounds.pos = glm::ivec2(
					roomToConnectToBounds.pos.x + Random(-room.bounds.size.x + 1, roomToConnectToBounds.size.x - 1),
					roomToConnectToBounds.pos.y + roomToConnectToBounds.size.y + m_minWallSize.y);
				break;
			}
			case 1:
			{
				// right
				room.bounds.pos = glm::ivec2(
					roomToConnectToBounds.pos.x + roomToConnectToBounds.size.x + m_minWallSize.x,
					roomToConnectToBounds.pos.y + Random(-room.bounds.size.y + 1, roomToConnectToBounds.size.y - 1));
				break;
			}
			case 2:
			{
				// bottom
				room.bounds.pos = glm::ivec2(
					roomToConnectToBounds.pos.x + Random(-room.bounds.size.x + 1, roomToConnectToBounds.size.x - 1),
					roomToConnectToBounds.pos.y - room.bounds.size.y - m_minWallSize.y);
				break;
			}
			case 3:
			{
				// left
				room.bounds.pos = glm::ivec2(
					roomToConnectToBounds.pos.x - room.bounds.size.x - m_minWallSize.x,
					roomToConnectToBounds.pos.y + Random(-room.bounds.size.y + 1, roomToConnectToBounds.size.y - 1));
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
		if (dist2.x < 0 && dist2.y < 0)
			return false;

		// touching
		if (dist2.x <= 0 && dist2.y <= 0)
			continue;

		// gap too small
		if (dist2.x < m_minWallSize.x && dist2.y < m_minWallSize.y)
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
		if (dist2.x < m_minWallSize.x && dist2.y < m_minWallSize.y)
			return false;
	}

	return true;
}

void DungeonGenerator::GenerateConnectionToParent(const Room& room)
{
	if (room.parent == -1)
		return;

	// Determine direction to parent
	int dir = -1; // 0 = horizontal, 1 = vertical
	const Room& parent = m_rooms[room.parent];

	if (room.bounds.pos.x + room.bounds.size.x < parent.bounds.pos.x ||
		room.bounds.pos.x > parent.bounds.pos.x + parent.bounds.size.x)
	{
		dir = 0;
	}
	else if (room.bounds.pos.y + room.bounds.size.y < parent.bounds.pos.y ||
		room.bounds.pos.y > parent.bounds.pos.y + parent.bounds.size.y)
	{
		dir = 1;
	}

	// Get rects from parent and own closest to each other
	int closestDist = std::numeric_limits<int>::max();
	Rect closestOwnRect = {};
	Rect closestParentRect = {};

	for (int i = 0; i < room.rects.size(); ++i)
	{
		for (int j = 0; j < parent.rects.size(); ++j)
		{
			const auto dist = DistanceBetweenRects(room.rects[i], parent.rects[j]);

			if (dist.x < 0 && dist.y < closestDist)
			{
				closestDist = dist.y;
				closestOwnRect = room.rects[i];
				closestParentRect = parent.rects[j];
			}
			if (dist.y < 0 && dist.x < closestDist)
			{
				closestDist = dist.x;
				closestOwnRect = room.rects[i];
				closestParentRect = parent.rects[j];
			}
		}
	}

	// Choose random pos to place corridor
	if (dir == 1)
	{
		const int minX = std::max(closestOwnRect.pos.x, closestParentRect.pos.x);
		const int maxX = std::min(closestOwnRect.pos.x + closestOwnRect.size.x, closestParentRect.pos.x + closestParentRect.size.x) - 1;

		int xpos = Random(minX, maxX);

		const int startY = std::min(closestOwnRect.pos.y + closestOwnRect.size.y, closestParentRect.pos.y + closestParentRect.size.y);
		const int endY = std::max(closestOwnRect.pos.y, closestParentRect.pos.y);

		// Add corridor
		for (int y = startY; y < endY; ++y)
		{
			glm::ivec2 pos = glm::ivec2(xpos, y);
			m_floorTiles.emplace(pos);

			// if would make too small gap, fill with floor
			if (m_floorTiles.find(pos + glm::ivec2(m_minWallSize.x, 0)) != m_floorTiles.end())
			{
				for (int x = pos.x; x < pos.x + m_minWallSize.x; ++x)
				{
					m_floorTiles.emplace(glm::ivec2(x, y));
				}
			}
			if (m_floorTiles.find(pos + glm::ivec2(-m_minWallSize.x, 0)) != m_floorTiles.end())
			{
				for (int x = pos.x - m_minWallSize.x; x < pos.x; ++x)
				{
					m_floorTiles.emplace(glm::ivec2(x, y));
				}
			}
		}
	}
	else if (dir == 0)
	{
		const int minY = std::max(closestOwnRect.pos.y, closestParentRect.pos.y);
		const int maxY = std::min(closestOwnRect.pos.y + closestOwnRect.size.y, closestParentRect.pos.y + closestParentRect.size.y) - 1;

		int ypos = Random(minY, maxY);
		ypos = minY;

		const int startX = std::min(closestOwnRect.pos.x + closestOwnRect.size.x, closestParentRect.pos.x + closestParentRect.size.x);
		const int endX = std::max(closestOwnRect.pos.x, closestParentRect.pos.x);

		// Add corridor
		for (int x = startX; x < endX; ++x)
		{
			glm::ivec2 pos = glm::ivec2(x, ypos);
			m_floorTiles.emplace(pos);

			// if would make too small gap, fill with floor
			if (m_floorTiles.find(pos + glm::ivec2(0, m_minWallSize.y)) != m_floorTiles.end())
			{
				for (int y = pos.y; y < pos.y + m_minWallSize.y; ++y)
				{
					m_floorTiles.emplace(glm::ivec2(x, y));
				}
			}
			if (m_floorTiles.find(pos + glm::ivec2(0, -m_minWallSize.y)) != m_floorTiles.end())
			{
				for (int y = pos.y - m_minWallSize.y; y < pos.y; ++y)
				{
					m_floorTiles.emplace(glm::ivec2(x, y));
				}
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