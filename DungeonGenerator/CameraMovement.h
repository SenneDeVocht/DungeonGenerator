#pragma once
#include <Mage/Components/Component.h>

namespace Mage
{
	class Transform;
}

class CameraMovement final : public Mage::Component
{
public:
	CameraMovement(Mage::Transform* target);
	void Update() override;

private:
	Mage::Transform* m_pTarget;
	float m_snappyness = 0.1f;
};

