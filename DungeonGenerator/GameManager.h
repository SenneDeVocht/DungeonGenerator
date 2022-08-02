#pragma once
#include <Mage/Components/Component.h>

class DungeonGenerator;
class PlayerMovement;

class GameManager final : public Mage::Component
{
public:
	GameManager(DungeonGenerator* pGenerator, PlayerMovement* pPlayerMovement);

	void Initialize() override;
	void Update() override;

private:
	void Reset() const;

	DungeonGenerator* m_pGenerator;
	PlayerMovement* m_pPlayerMovement;
};

