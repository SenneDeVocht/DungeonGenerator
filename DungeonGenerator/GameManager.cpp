#include "pch.h"
#include "GameManager.h"

#include <chrono>
#include <Mage/Components/Transform.h>
#include <Mage/Engine/ServiceLocator.h>
#include <Mage/Input/InputManager.h>

#include "DungeonGenerator.h"
#include "DungeonDrawer.h"
#include "PlayerMovement.h"

GameManager::GameManager(DungeonGenerator* pGenerator, DungeonDrawer* pDrawer, PlayerMovement* pPlayerMovement, Mage::Transform* pCameraTransform)
	: m_pGenerator(pGenerator)
	, m_pDrawer(pDrawer)
	, m_pPlayerMovement(pPlayerMovement)
	, m_pCameraTransform(pCameraTransform)
{}

void GameManager::Initialize()
{
	Reset();
}

void GameManager::Update()
{
	if (m_pPlayerMovement->GetPosition() == m_pGenerator->GetExitPos())
	{
		Reset();
	}
}

void GameManager::Reset() const
{
	// Generate a new dungeon
	const auto start = std::chrono::high_resolution_clock::now();

	const unsigned int seed = rand();
	m_pGenerator->GenerateDungeon(seed);

	const auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Generation time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
	std::cout << std::endl;

	// Draw the dungeon
	m_pDrawer->DrawDungeon(m_pGenerator);

	// Place player and camera at start of dungeon
	// new camera position
	const glm::vec2 newCameraPos = glm::vec2(m_pGenerator->GetStartPos()) + m_pCameraTransform->GetWorldPosition() - glm::vec2(m_pPlayerMovement->GetPosition());
	m_pCameraTransform->SetWorldPosition(newCameraPos);

	// new player position
	m_pPlayerMovement->SetPosition(m_pGenerator->GetStartPos());
}
