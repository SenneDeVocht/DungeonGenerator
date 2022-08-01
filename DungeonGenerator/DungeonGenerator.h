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
	void Initialize() override;
	void Update() override;
	void RenderGizmos() const override;

private:
	struct Rect
	{
		glm::ivec2 pos{};
		glm::ivec2 size{};
	};

	struct Room
	{
		std::vector<Rect> rects{};
		Rect bounds{};
		int parent{ -1 };
	};

	void GenerateDungeon();
	void GenerateRoom();
	void SetRoomPosition(Room& room) const;
	bool CanAddRectToRoom(const Room& room, const Rect& rect) const;
	bool CanAddRoomToDungeon(const Rect& bounds) const;
	void GenerateConnectionToParent(const Room& room);
	void DrawDungeon() const;

	glm::ivec2 DistanceBetweenRects(const Rect& rect1, const Rect& rect2) const;
	static Rect GetRoomBounds(const Room& room);

	// Settings
	const int m_roomAmount{ 10 };
	const int m_minRoomRectSize{ 3 };
	const int m_maxRoomRectSize{ 6 };
	const int m_minRoomRects{ 2 };
	const int m_maxRoomRects{ 5 };
	const bool m_roomRectsCanOverlap{ false };
	const glm::ivec2 m_minHoleSize{ 2, 3 };

	std::vector<Room> m_rooms{};
	std::unordered_set<glm::ivec2> m_floorTiles{};
	Mage::TilemapComponent* m_pTilemap{};
};
