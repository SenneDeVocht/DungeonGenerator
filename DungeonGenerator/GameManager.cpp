#include "pch.h"
#include "GameManager.h"

#include <Mage/Engine/ServiceLocator.h>
#include <Mage/Input/InputManager.h>

#include "DungeonGenerator.h"
#include "PlayerMovement.h"

GameManager::GameManager(DungeonGenerator* pGenerator, PlayerMovement* pPlayerMovement)
	: m_pGenerator(pGenerator)
	, m_pPlayerMovement(pPlayerMovement)
{}

void GameManager::Initialize()
{
	Reset();
}

void GameManager::Update()
{
	const auto input = Mage::ServiceLocator::GetInputManager();

	if (input->CheckKeyboardKey('R', Mage::InputState::Down))
		Reset();
}

void GameManager::Reset() const
{
	m_pGenerator->GenerateDungeon();
	m_pPlayerMovement->SetPosition({ 0, 0 });
}
