#pragma once
#include <Mage/Components/Component.h>

class DungeonDrawer;
class DungeonGenerator;
class PlayerMovement;

class GameManager final : public Mage::Component
{
public:
	GameManager(DungeonGenerator* pGenerator, DungeonDrawer* drawer, PlayerMovement* pPlayerMovement);

	void Initialize() override;
	void Update() override;

private:
	void Reset() const;

	DungeonGenerator* m_pGenerator;
	DungeonDrawer* m_pDrawer;
	PlayerMovement* m_pPlayerMovement;
};

