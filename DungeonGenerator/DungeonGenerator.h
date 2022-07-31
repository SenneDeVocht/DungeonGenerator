#pragma once
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
	};

	void GenerateDungeon();
	Room GenerateRoom() const;
	bool CanAddRectToRoom(const Room& room, const Rect& rect) const;
	void DrawDungeon() const;

	glm::ivec2 DistanceBetweenRects(const Rect& r1, const Rect& r2) const;

	// Settings
	const int m_roomAmount{ 1 };
	const int m_minRoomRectSize{ 3 };
	const int m_maxRoomRectSize{ 6 };
	const int m_minRoomRects{ 3 };
	const int m_maxRoomRects{ 6 };
	const bool m_roomRectsCanOverlap{ false };
	const glm::ivec2 m_minHoleSize{ 2, 3 };

	std::vector<Room> m_rooms{};
	Mage::TilemapComponent* m_pTilemap{};
};
