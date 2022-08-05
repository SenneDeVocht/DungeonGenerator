#pragma once
#include <Mage/Components/Component.h>

namespace Mage
{
	class Transform;
}

class DungeonDrawer;
class DungeonGenerator;
class PlayerMovement;

class GameManager final : public Mage::Component
{
public:
	GameManager(DungeonGenerator* pGenerator, DungeonDrawer* pDrawer, PlayerMovement* pPlayerMovement, Mage::Transform* pCameraTransform);

	void Initialize() override;
	void Update() override;

private:
	void Reset() const;

	DungeonGenerator* m_pGenerator;
	DungeonDrawer* m_pDrawer;
	PlayerMovement* m_pPlayerMovement;
	Mage::Transform* m_pCameraTransform;
};

