#include "pch.h"
#include "GameManager.h"

#include <chrono>
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
	// Generate a new dungeon
	const auto start = std::chrono::high_resolution_clock::now();

	m_pGenerator->GenerateDungeon();

	const auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Generation time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
	std::cout << std::endl;

	// Place player at start of dungeon
	m_pPlayerMovement->SetPosition({ 0, 0 });
}
