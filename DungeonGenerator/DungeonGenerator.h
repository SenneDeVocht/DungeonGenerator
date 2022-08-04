#pragma once
#include <unordered_set>
#include <Mage/Components/Component.h>

namespace Mage
{
	class TilemapComponent;
}

class DungeonGenerator final : public Mage::Component
{
public:
	void RenderGizmos() const override;

	struct Rect
	{
		glm::ivec2 pos{};
		glm::ivec2 size{};
	};

	struct Room
	{
		std::vector<Rect> rects{};
		Rect bounds{};
	};

	struct hashFunction
	{
		size_t operator()(const std::pair<int, int>& x) const
		{
			return x.first ^ x.second;
		}
	};

	void GenerateDungeon();
	const std::unordered_set<glm::ivec2>& GetFloorTiles() { return m_floorTiles; }
	const std::vector<Room>& GetRooms() { return m_rooms; }
	const std::unordered_set<std::pair<int, int>, hashFunction>& GetConnections() { return m_connections; }

private:
	void ClearDungeon();
	void GenerateRoom();
	glm::ivec2 FindPositionForRoom(const Room& room) const;
	void GenerateConnections();
	void GenerateConnection(const Room& room1, const Room& room2);
	bool CanAddRectToRoom(const Room& room, const Rect& rect) const;
	bool CanAddRoomToDungeon(const Rect& bounds) const;
	glm::ivec2 DistanceBetweenRects(const Rect& rect1, const Rect& rect2) const;
	static Rect GetRoomBounds(const Room& room);

	// Settings
	const int m_roomAmount{ 10 };
	const int m_minRoomRectSize{ 5 };
	const int m_maxRoomRectSize{ 7 };
	const int m_minRoomRects{ 2 };
	const int m_maxRoomRects{ 5 };
	const glm::ivec2 m_minWallSize{ 2, 3 };
	const glm::ivec2 m_minRoomDistance{ 4, 5 };
	const float m_extraConnectionChance{ 0.5f };

	std::vector<Room> m_rooms{};
	std::unordered_set<std::pair<int, int>, hashFunction> m_connections{};
	std::unordered_set<glm::ivec2> m_floorTiles{};

	bool m_isValid{ true };
};
