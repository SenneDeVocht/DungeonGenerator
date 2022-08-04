#include "pch.h"
#include "GameManager.h"

#include <chrono>
#include <Mage/Engine/ServiceLocator.h>
#include <Mage/Input/InputManager.h>

#include "DungeonGenerator.h"
#include "DungeonDrawer.h"
#include "PlayerMovement.h"

GameManager::GameManager(DungeonGenerator* pGenerator, DungeonDrawer* drawer, PlayerMovement* pPlayerMovement)
	: m_pGenerator(pGenerator)
	, m_pDrawer(drawer)
	, m_pPlayerMovement(pPlayerMovement)
{}

void GameManager::Initialize()
{
	Reset();
}

void GameManager::Update()
{
	if (m_pPlayerMovement->GetPosition() == m_pGenerator->GetExitPos())
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

	// Draw the dungeon
	m_pDrawer->DrawDungeon(m_pGenerator);

	// Place player at start of dungeon
	m_pPlayerMovement->SetPosition(m_pGenerator->GetStartPos());
}
