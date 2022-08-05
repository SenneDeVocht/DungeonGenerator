#pragma once
#include <random>
#include <unordered_set>
#include <Mage/Components/Component.h>

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

	struct Connection
	{
		std::pair<int, int> rooms{};
		float distance{};

		bool operator==(const Connection& other) const
		{
			return rooms == other.rooms;
		}
	};

	struct ConnectionHash
	{
		size_t operator()(const Connection& x) const
		{
			return x.rooms.first ^ x.rooms.second;
		}
	};

	void GenerateDungeon(unsigned int seed);
	const std::unordered_set<glm::ivec2>& GetFloorTiles() const { return m_floorTiles; }
	const std::vector<Room>& GetRooms() const { return m_rooms; }
	const std::unordered_set<Connection, ConnectionHash>& GetConnections() const { return m_connections; }
	const glm::ivec2& GetStartPos() const { return m_startPos; }
	const glm::ivec2& GetExitPos() const { return m_exitPos; }
	const Rect& GetDungeonBounds() const { return m_dungeonBounds; }

private:
	void ClearDungeon();
	void GenerateRoom();
	glm::ivec2 FindPositionForRoom(const Room& room) const;
	void GenerateConnections();
	void GenerateConnection(const Room& room1, const Room& room2);
	void ChooseStartAndExit();
	void CalculateDungeonBounds();

	bool CanAddRectToRoom(const Room& room, const Rect& rect) const;
	bool CanAddRoomToDungeon(const Rect& bounds) const;
	glm::ivec2 DistanceBetweenRects(const Rect& rect1, const Rect& rect2) const;
	static Rect GetRoomBounds(const Room& room);
	int RandomInt(int min, int max);

	// Settings
	const int m_roomAmount{ 10 };
	const int m_minRoomRectSize{ 5 };
	const int m_maxRoomRectSize{ 7 };
	const int m_minRoomRects{ 2 };
	const int m_maxRoomRects{ 5 };
	const glm::ivec2 m_minWallSize{ 2, 3 };
	const glm::ivec2 m_minRoomDistance{ 4, 5 };
	const float m_extraConnectionChance{ 0.5f };
	const int m_wantedRoomsBetweenStartAndExit{ 2 };

	// Output
	std::vector<Room> m_rooms{};
	std::unordered_set<Connection, ConnectionHash> m_connections{};
	std::unordered_set<glm::ivec2> m_floorTiles{};
	glm::ivec2 m_startPos{};
	glm::ivec2 m_exitPos{};
	Rect m_dungeonBounds{};


	bool m_isValid{ true };
	std::mt19937 m_rng{};
};
