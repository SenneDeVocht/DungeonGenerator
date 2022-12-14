#include "pch.h"
#include "DungeonGenerator.h"

#include <queue>
#include <random>
#include <Mage/Components/TilemapComponent.h>
#include <Mage/Engine/ServiceLocator.h>
#include <Mage/Scenegraph/GameObject.h>
#include <Mage/Engine/Renderer.h>

int DungeonGenerator::RandomInt(int min, int max)
{
	return m_rng() % (max - min + 1) + min;
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
			renderer->RenderPolygonFilled(corners, glm::vec4(0.f, 1.f, 1.f, 0.5f), 1.f);
		}

		// Bounds
		std::vector<glm::vec2> corners = {
				glm::vec2(room.bounds.pos.x - 0.5f, room.bounds.pos.y - 0.5f),
				glm::vec2(room.bounds.pos.x + room.bounds.size.x - 0.5f, room.bounds.pos.y - 0.5f),
				glm::vec2(room.bounds.pos.x + room.bounds.size.x - 0.5f, room.bounds.pos.y + room.bounds.size.y - 0.5f),
				glm::vec2(room.bounds.pos.x - 0.5f, room.bounds.pos.y + room.bounds.size.y - 0.5f)
		};

		renderer->RenderPolygonOutline(corners, true, glm::vec4(1.f, 0.f, 0.f, 1.0f), 1.f);
	}

	// Connections
	for (const auto& connection : m_connections)
	{
		int r1 = connection.rooms.first;
		int r2 = connection.rooms.second;

		std::vector<glm::vec2> points = {
			glm::vec2(m_rooms[r1].bounds.pos) + glm::vec2(m_rooms[r1].bounds.size) / 2.0f - 0.5f,
			glm::vec2(m_rooms[r2].bounds.pos) + glm::vec2(m_rooms[r2].bounds.size) / 2.0f - 0.5f
		};

		renderer->RenderPolygonOutline(points, false, glm::vec4(0.f, 1.f, 0.f, 1.f), 1.f);
	}
}

void DungeonGenerator::GenerateDungeon(unsigned int seed)
{
	m_rng.seed(seed);
	std::cout << "Seed: " << seed << std::endl;

	do
	{
		std::cout << "Starting generation..." << std::endl;

		// Clear previous
		ClearDungeon();

		// Generate Rooms
		for (int i = 0; i < m_roomAmount; i++)
		{
			GenerateRoom();
		}

		// Generate Connections
		GenerateConnections();
	} while (!m_isValid);

	ChooseStartAndExit();

	CalculateDungeonBounds();
}

void DungeonGenerator::ClearDungeon()
{
	m_rooms.clear();
	m_connections.clear();
	m_floorTiles.clear();
	m_dungeonBounds = Rect{ {0, 0}, {0, 0} };
	m_isValid = true;
}

void DungeonGenerator::GenerateRoom()
{
	Room room;

	// Root rectangle
	room.rects.push_back({
		glm::vec2(0, 0),
		glm::vec2(
			RandomInt(m_minRoomRectSize, m_maxRoomRectSize),
			RandomInt(m_minRoomRectSize, m_maxRoomRectSize))
	});

	// Additional rectangles
	const int amountExtraRects = RandomInt(m_minRoomRects, m_maxRoomRects) - 1;
	for (int i = 0; i < amountExtraRects; i++)
	{
		Rect addRect;

		// keep generating rect until it can be added
		do
		{
			const Rect& rectToConnectTo = room.rects[RandomInt(0, (int)room.rects.size() - 1)];

			addRect.size = glm::vec2(
				RandomInt(m_minRoomRectSize, m_maxRoomRectSize),
				RandomInt(m_minRoomRectSize, m_maxRoomRectSize));

			// set position so it shares a corner with root rect
			const int corner = RandomInt(0, 7);
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
	glm::ivec2 offset = room.bounds.pos;
	glm::ivec2 roomPos = FindPositionForRoom(room);
	room.bounds.pos = roomPos;
	for (auto& rect : room.rects)
	{
		rect.pos += roomPos - offset;
	}

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

glm::ivec2 DungeonGenerator::FindPositionForRoom(const Room& room) const
{
	// place first room in center
	if (m_rooms.empty())
		return -room.bounds.size / 2;

	// find bounds of dungeon
	glm::ivec2 min = { std::numeric_limits<int>::max(), std::numeric_limits<int>::max() };
	glm::ivec2 max = { std::numeric_limits<int>::min(), std::numeric_limits<int>::min() };
	for (const auto& r : m_rooms)
	{
		if (r.bounds.pos.x < min.x)
			min.x = r.bounds.pos.x;
		if (r.bounds.pos.y < min.y)
			min.y = r.bounds.pos.y;
		if (r.bounds.pos.x + r.bounds.size.x > max.x)
			max.x = r.bounds.pos.x + r.bounds.size.x;
		if (r.bounds.pos.y + r.bounds.size.y > max.y)
			max.y = r.bounds.pos.y + r.bounds.size.y;
	}

	// leave space for walls
	min -= m_minRoomDistance + room.bounds.size;
	max += m_minRoomDistance;

	// find free position closest to [0, 0]
	Rect bounds = room.bounds;
	glm::ivec2 closestPos = { 0, 0 };
	float closestDist = FLT_MAX;
	for (int x = min.x; x < max.x; x++)
	{
		for (int y = min.y; y < max.y; y++)
		{
			// See if closer
			const float dist = glm::distance(glm::vec2(0, 0), glm::vec2(x, y) + glm::vec2(bounds.size) * 0.5f);
			if (dist >= closestDist)
				continue;

			// test if can place room
			bounds.pos = glm::ivec2(x, y);
			if (CanAddRoomToDungeon(bounds))
			{
				closestDist = dist;
				closestPos = glm::vec2(x, y);
			}
		}
	}

	return closestPos;
}

void DungeonGenerator::GenerateConnections()
{
	// Get possible connections for each room
	// (rooms can connect to the rooms closest by)
	std::vector<Connection> possibleConnections;

	for (int i = 0; i < (int)m_rooms.size(); i++)
	{
		// get closest rooms
		std::vector<int> closestRooms;
		int closestDist = std::numeric_limits<int>::max();

		for (int j = 0; j < (int)m_rooms.size(); j++)
		{
			// skip self
			if (i == j)
				continue;

			// get distance
			glm::ivec2 dist2 = DistanceBetweenRects(m_rooms[i].bounds, m_rooms[j].bounds);

			// skip if not overlapping on x or y
			if (dist2.x >= 0 && dist2.y >= 0)
				continue;

			// collect room if closest
			dist2 -= m_minRoomDistance;
			const int dist = std::max(dist2.x, dist2.y);

			if (dist < closestDist)
			{
				closestRooms = { j };
				closestDist = dist;
			}
			else if (dist == closestDist)
			{
				closestRooms.push_back(j);
			}
		}

		// add connections
		for (int j : closestRooms)
		{
			// avoid duplicates
			const auto it = std::find_if(
				possibleConnections.begin(), possibleConnections.end(), 
				[&](const Connection& c)
				{
					return c.rooms == std::make_pair(j, i);
				}
			);

			if (it == possibleConnections.end())
			{
				const float weight = glm::length(glm::vec2(m_rooms[i].bounds.pos) + glm::vec2(m_rooms[j].bounds.size) * 0.5f);
				possibleConnections.emplace_back(Connection{ std::make_pair(i, j), weight });
			}
		}
	}

	// Create minimum spanning tree using Prim's algorithm
	std::unordered_set<int> visited{ 0 };
	std::unordered_set<int> spanningTreeIndices{};
	
	while (visited.size() < m_rooms.size())
	{
		// find closest node
		int smallestEdgeIndex = {};
		float smallestWeight = std::numeric_limits<float>::max();
		for (int j = 0; j < (int)possibleConnections.size(); j++)
		{
			// skip if both nodes visited, or both nodes not visited
			if (visited.find(possibleConnections[j].rooms.first) != visited.end() && visited.find(possibleConnections[j].rooms.second) != visited.end() ||
				visited.find(possibleConnections[j].rooms.first) == visited.end() && visited.find(possibleConnections[j].rooms.second) == visited.end())
				continue;

			if (possibleConnections[j].distance < smallestWeight)
			{
				smallestWeight = possibleConnections[j].distance;
				smallestEdgeIndex = j;
			}
		}

		// no edge found, but not all nodes visited
		// this will probably never happen, but just in case
		if (smallestEdgeIndex == -1)
		{
			m_isValid = false;
			return;
		}

		// add connection to tree, and mark rooms as visited
		spanningTreeIndices.insert(smallestEdgeIndex);
		visited.insert(possibleConnections[smallestEdgeIndex].rooms.first);
		visited.insert(possibleConnections[smallestEdgeIndex].rooms.second);
	}

	// Add tree to connections
	for (const int i : spanningTreeIndices)
	{
		m_connections.insert(possibleConnections[i]);
	}

	// Add extra connections
	for (Connection c : possibleConnections)
	{
		const float rnd = m_rng() / float(m_rng.max() - m_rng.min());
		if (rnd < m_extraConnectionChance)
		{
			m_connections.insert(c);
		}
	}

	// Connect rooms
	for (const auto& connection : m_connections)
	{
		GenerateConnection(m_rooms[connection.rooms.first], m_rooms[connection.rooms.second]);
	}
}

void DungeonGenerator::GenerateConnection(const Room& room1, const Room& room2)
{
	// Determine direction of corridor
	int dir = -1; // 0 = horizontal, 1 = vertical

	if (room1.bounds.pos.x + room1.bounds.size.x < room2.bounds.pos.x ||
		room1.bounds.pos.x > room2.bounds.pos.x + room2.bounds.size.x)
	{
		dir = 0;
	}
	else if (room1.bounds.pos.y + room1.bounds.size.y < room2.bounds.pos.y ||
		room1.bounds.pos.y > room2.bounds.pos.y + room2.bounds.size.y)
	{
		dir = 1;
	}

	// Get two rects closest to each other
	int closestDist = std::numeric_limits<int>::max();
	Rect closestOwnRect = {};
	Rect closestParentRect = {};

	for (auto& rect1 : room1.rects)
	{
		for (auto& rect2 : room2.rects)
		{
			const auto dist = DistanceBetweenRects(rect1, rect2);

			if (dist.x < 0 && dist.y < closestDist)
			{
				closestDist = dist.y;
				closestOwnRect = rect1;
				closestParentRect = rect2;
			}
			if (dist.y < 0 && dist.x < closestDist)
			{
				closestDist = dist.x;
				closestOwnRect = rect1;
				closestParentRect = rect2;
			}
		}
	}

	if (dir == 1)
	{
		// Choose pos to place corridor
		const int minX = std::max(closestOwnRect.pos.x, closestParentRect.pos.x);
		const int maxX = std::min(closestOwnRect.pos.x + closestOwnRect.size.x, closestParentRect.pos.x + closestParentRect.size.x) - 1;

		int xpos = RandomInt(minX, maxX);

		const int startY = std::min(closestOwnRect.pos.y + closestOwnRect.size.y, closestParentRect.pos.y + closestParentRect.size.y);
		const int endY = std::max(closestOwnRect.pos.y, closestParentRect.pos.y);

		// Add corridor
		for (int y = startY; y < endY; ++y)
		{
			glm::ivec2 pos = glm::ivec2(xpos, y);
			m_floorTiles.emplace(pos);

			// if would make too small gap, fill with floor
			for (int offset = m_minWallSize.x; offset > 0; --offset)
			{
				if (m_floorTiles.find(pos + glm::ivec2(offset, 0)) != m_floorTiles.end())
				{
					m_floorTiles.emplace(pos + glm::ivec2(offset - 1, 0));
				}
				if (m_floorTiles.find(pos + glm::ivec2(-offset, 0)) != m_floorTiles.end())
				{
					m_floorTiles.emplace(pos + glm::ivec2(-offset + 1, 0));
				}
			}
		}
	}
	else if (dir == 0)
	{
		// Choose pos to place corridor
		const int minY = std::max(closestOwnRect.pos.y, closestParentRect.pos.y);
		const int maxY = std::min(closestOwnRect.pos.y + closestOwnRect.size.y, closestParentRect.pos.y + closestParentRect.size.y) - 1;

		int ypos = RandomInt(minY, maxY);
		ypos = minY;

		const int startX = std::min(closestOwnRect.pos.x + closestOwnRect.size.x, closestParentRect.pos.x + closestParentRect.size.x);
		const int endX = std::max(closestOwnRect.pos.x, closestParentRect.pos.x);

		// Add corridor
		for (int x = startX; x < endX; ++x)
		{
			glm::ivec2 pos = glm::ivec2(x, ypos);
			m_floorTiles.emplace(pos);

			// if would make too small gap, fill with floor
			for (int offset = m_minWallSize.y; offset > 0; --offset)
			{
				if (m_floorTiles.find(pos + glm::ivec2(0, offset)) != m_floorTiles.end())
				{
					m_floorTiles.emplace(pos + glm::ivec2(0, offset - 1));
				}
				if (m_floorTiles.find(pos + glm::ivec2(0, -offset)) != m_floorTiles.end())
				{
					m_floorTiles.emplace(pos + glm::ivec2(0, -offset + 1));
				}
			}
		}
	}
}

void DungeonGenerator::ChooseStartAndExit()
{
	struct Node
	{
		std::vector<int> connectedNodes;
	};

	// construct easier to traverse graph of rooms
	std::vector<Node> graph;

	for (int i = 0; i < m_rooms.size(); ++i)
	{
		graph.emplace_back();
	}

	for (const auto& connection : m_connections)
	{
		graph[connection.rooms.first].connectedNodes.push_back(connection.rooms.second);
		graph[connection.rooms.second].connectedNodes.push_back(connection.rooms.first);
	}

	// find distance on graph between all pairs of rooms
	std::vector<std::vector<int>> distances(graph.size(), std::vector<int>(graph.size(), 0));

	for (int i = 0; i < graph.size(); ++i)
	{
		// find shortest path between rooms using breadth first search
		std::queue<int> q;
		q.push(i);
		std::unordered_set<int> visited;

		while (!q.empty())
		{
			const int curr = q.front();
			q.pop();

			for (const auto& connectedNode : graph[curr].connectedNodes)
			{
				// not yet visited
				if (visited.find(connectedNode) == visited.end())
				{
					q.push(connectedNode);
					visited.emplace(connectedNode);
					distances[i][connectedNode] = distances[i][curr] + 1;
				}
			}
		}
	}

	// find rooms far enough apart
	int minDist = m_wantedRoomsBetweenStartAndExit + 1;
	std::vector<std::pair<int, int>> pairsToConsider{};
	while (pairsToConsider.empty())
	{
		for (int i = 0; i < graph.size(); ++i)
		{
			for (int j = i + 1; j < graph.size(); ++j)
			{
				if (distances[i][j] >= minDist)
				{
					pairsToConsider.emplace_back(i, j);
				}
			}
		}

		--minDist;
	}

	// choose random pair
	std::pair<int, int> chosenRooms = pairsToConsider[RandomInt(0, (int)pairsToConsider.size() - 1)];
	const Room& room1 = m_rooms[chosenRooms.first];
	const Room& room2 = m_rooms[chosenRooms.second];

	// Choose pos in random rect of room 1 to be the start
	const Rect& startRect = room1.rects[RandomInt(0, (int)room1.rects.size() - 1)];

	m_startPos = glm::ivec2(RandomInt(startRect.pos.x, startRect.pos.x + startRect.size.x - 1),
		RandomInt(startRect.pos.y, startRect.pos.y + startRect.size.y - 1));

	// Choose pos in random rect of room 2 to be the exit
	const Rect& exitRect = room2.rects[RandomInt(0, (int)room2.rects.size() - 1)];

	m_exitPos = glm::ivec2(RandomInt(exitRect.pos.x, exitRect.pos.x + exitRect.size.x - 1),
		RandomInt(exitRect.pos.y, exitRect.pos.y + exitRect.size.y - 1));
}

void DungeonGenerator::CalculateDungeonBounds()
{
	glm::ivec2 minPos{ std::numeric_limits<int>::max(), std::numeric_limits<int>::max() };
	glm::ivec2 maxPos{ std::numeric_limits<int>::min(), std::numeric_limits<int>::min() };

	for (const auto& room : m_rooms)
	{
		for (const auto& rect : room.rects)
		{
			minPos.x = std::min(minPos.x, rect.pos.x);
			minPos.y = std::min(minPos.y, rect.pos.y);
			maxPos.x = std::max(maxPos.x, rect.pos.x + rect.size.x - 1);
			maxPos.y = std::max(maxPos.y, rect.pos.y + rect.size.y - 1);
		}
	}

	m_dungeonBounds = Rect{ minPos, maxPos - minPos + glm::ivec2(1, 1) };
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
		if (dist2.x < m_minRoomDistance.x && dist2.y < m_minRoomDistance.y)
			return false;
	}

	return true;
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
