#pragma once
#include <Mage/Components/Component.h>

namespace Mage
{
	class CameraComponent;
}

class CameraMovement final : public Mage::Component
{
public:
	void Initialize() override;
	void Update() override;

private:
	Mage::CameraComponent* m_pCamera;

	float m_camSize{ 50.f };
	const float m_zoomSpeed{ 0.5f };
	const float m_speed{ 5.f };
};

