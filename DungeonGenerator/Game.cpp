#include "pch.h"
#include "Game.h"

#include <Mage/Components/CameraComponent.h>
#include <Mage/Components/TilemapComponent.h>
#include <Mage/Components/SpriteComponent.h>

#include <Mage/Engine/ServiceLocator.h>
#include <Mage/Engine/Renderer.h>
#include <Mage/Scenegraph/SceneManager.h>
#include <Mage/Scenegraph/Scene.h>
#include <Mage/Scenegraph/GameObject.h>
#include <Mage/ResourceManagement/ResourceManager.h>

#include <Mage/structs.h>

#include "CameraMovement.h"
#include "DungeonGenerator.h"
#include "DungeonDrawer.h"
#include "GameManager.h"
#include "PlayerMovement.h"

void Game::LoadGame() const
{
	const auto& resourceManager = Mage::ResourceManager::GetInstance();

	Mage::SceneManager::GetInstance().RegisterScene("Test", [&](Mage::Scene* pScene)
	{
		// Dungeon Generator
		//------------------
		#pragma region Dungeon Generator

		const auto dungeonGeneratorObject = pScene->CreateChildObject("Dungeon Generator");
		dungeonGeneratorObject->CreateComponent<Mage::TilemapComponent>(
			std::vector<std::shared_ptr<Mage::Texture2D>> {
				resourceManager.LoadTexture("Sprites/DungeonTiles/floor.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/floor_variation_1.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/floor_variation_2.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/floor_shadow_insidecorner_bottomleft.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/floor_shadow_insidecorner_topleft.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/floor_shadow_outsidecorner.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/floor_shadow_straight_left.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/floor_shadow_straight_top.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/floor_shadow_straight_top_variation.png", 16),

				resourceManager.LoadTexture("Sprites/DungeonTiles/wall.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/wall_variation_1.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/wall_variation_2.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/wall_left.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/wall_right.png", 16),

				resourceManager.LoadTexture("Sprites/DungeonTiles/roof.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/roof_insidecorner_bottomleft.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/roof_insidecorner_bottomright.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/roof_insidecorner_topleft.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/roof_insidecorner_topright.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/roof_outsidecorner_bottomleft.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/roof_outsidecorner_bottomright.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/roof_outsidecorner_topleft.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/roof_outsidecorner_topright.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/roof_straight_bottom.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/roof_straight_left.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/roof_straight_right.png", 16),
				resourceManager.LoadTexture("Sprites/DungeonTiles/roof_straight_top.png", 16),

				resourceManager.LoadTexture("Sprites/DungeonTiles/exit.png", 16),
			}
		);
		const auto dungeonGenerator = dungeonGeneratorObject->CreateComponent<DungeonGenerator>();
		const auto dungeonDrawer = dungeonGeneratorObject->CreateComponent<DungeonDrawer>();

		#pragma endregion


		// Player
		//-------
		#pragma region Player

		const auto playerObject = pScene->CreateChildObject("Player");
		const auto playerMovement = playerObject->CreateComponent<PlayerMovement>(dungeonGenerator);
		playerObject->CreateComponent<Mage::SpriteComponent>(resourceManager.LoadTexture("Sprites/wizard.png", 16), 1.f);

		#pragma endregion


		// Camera
		//-------
		#pragma region Camera

		const auto cameraObject = pScene->CreateChildObject("Camera");
		const auto camera = cameraObject->CreateComponent<Mage::CameraComponent>(glm::vec2{ 25.f, 25.f });
		cameraObject->CreateComponent<CameraMovement>(playerObject->GetTransform());
		Mage::ServiceLocator::GetRenderer()->SetCamera(camera);
		Mage::ServiceLocator::GetRenderer()->SetBackgroundColor({ 118, 59, 54, 255 });

		#pragma endregion


		// Game Manager
		//-------------
		#pragma region Game Manager

		const auto gameManagerObject = pScene->CreateChildObject("Game Manager");
		gameManagerObject->CreateComponent<GameManager>(dungeonGenerator, dungeonDrawer, playerMovement);

		#pragma endregion
	});

	Mage::SceneManager::GetInstance().LoadScene("Test");
}
