#include "pch.h"
#include "CameraMovement.h"

#include <Mage/Components/CameraComponent.h>
#include <Mage/Engine/ServiceLocator.h>
#include <Mage/Input/InputManager.h>
#include <Mage/Scenegraph/GameObject.h>
#include <Mage/Engine/Timer.h>

void CameraMovement::Initialize()
{
	m_pCamera = GetGameObject()->GetComponent<Mage::CameraComponent>();
}

void CameraMovement::Update()
{
	const auto input = Mage::ServiceLocator::GetInputManager();

	// Get direction
	glm::vec2 dir = { 0, 0 };

	if (input->CheckKeyboardKey('A', Mage::InputState::Hold))
		dir += glm::vec2(-1, 0);

	if (input->CheckKeyboardKey('W', Mage::InputState::Hold))
		dir += glm::vec2(0, 1);

	if (input->CheckKeyboardKey('D', Mage::InputState::Hold))
		dir += glm::vec2(1, 0);

	if (input->CheckKeyboardKey('S', Mage::InputState::Hold))
		dir += glm::vec2(0, -1);

	// Move
	const float dt = Mage::Timer::GetInstance().GetDeltaTime();
	GetGameObject()->GetTransform()->Translate(dir * m_speed * dt);

	// Get zoom change
	float zoom = 1;

	if (input->CheckKeyboardKey('Q', Mage::InputState::Hold))
		zoom /= 1 + m_zoomSpeed * dt;

	if (input->CheckKeyboardKey('E', Mage::InputState::Hold))
		zoom *= 1 + m_zoomSpeed * dt;

	// Set cam size
	m_camSize *= zoom;
	m_pCamera->SetSize({ m_camSize, m_camSize });
}
