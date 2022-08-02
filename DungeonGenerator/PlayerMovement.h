#pragma once
#include <Mage/Components/Component.h>

class DungeonGenerator;

class PlayerMovement final : public Mage::Component
{
public:
	PlayerMovement(DungeonGenerator* dungeon);
	void Update() override;
	
	void SetPosition(glm::ivec2 position);

private:
	DungeonGenerator* m_pDungeon;
	glm::ivec2 m_position{ 0, 0 };

	float m_timeBetweenMoves{ 0.15f };
	float m_timeSinceLastMove{ 0.0f };
};

