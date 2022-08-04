#include "pch.h"
#include "PlayerMovement.h"

#include <Mage/Engine/ServiceLocator.h>
#include <Mage/Engine/Timer.h>
#include <Mage/Input/InputManager.h>
#include <Mage/Scenegraph/GameObject.h>

#include "DungeonGenerator.h"

PlayerMovement::PlayerMovement(DungeonGenerator* dungeon)
	: m_pDungeon(dungeon)
{}

void PlayerMovement::Update()
{
	// Move to position
	GetGameObject()->GetTransform()->SetWorldPosition(m_position + glm::ivec2(0.5f, 0.5f));

	// See if can move
	m_timeSinceLastMove += Mage::Timer::GetInstance().GetDeltaTime();
	if (m_timeSinceLastMove < m_timeBetweenMoves)
	{
		return;
	}

	// Get wanted direction
	const auto input = Mage::ServiceLocator::GetInputManager();
	glm::ivec2 dir = { 0, 0 };

	if (input->CheckKeyboardKey('W', Mage::InputState::Hold) ||
		input->CheckKeyboardKey(0x26, Mage::InputState::Hold))
	{
		dir.y += 1;
		m_timeSinceLastMove = 0;
	}
	else if (input->CheckKeyboardKey('A', Mage::InputState::Hold) ||
		input->CheckKeyboardKey(0x25, Mage::InputState::Hold))
	{
		dir.x -= 1;
		m_timeSinceLastMove = 0;
	}
	else if (input->CheckKeyboardKey('S', Mage::InputState::Hold) ||
		input->CheckKeyboardKey(0x28, Mage::InputState::Hold))
	{
		dir.y -= 1;
		m_timeSinceLastMove = 0;
	}
	else if (input->CheckKeyboardKey('D', Mage::InputState::Hold) ||
		input->CheckKeyboardKey(0x27, Mage::InputState::Hold))
	{
		dir.x += 1;
		m_timeSinceLastMove = 0;
	}

	// Move if able
	const auto floor = m_pDungeon->GetFloorTiles();
	if(floor.find(m_position + dir) != floor.end())
	{
		m_position += dir;
	}
}

void PlayerMovement::SetPosition(glm::ivec2 position)
{
	m_position = position;
}
