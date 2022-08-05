#include "pch.h"

#include <Mage/Engine/GameSettings.h>

#include "Game.h"

int main()
{
    // Settings
    Mage::GameSettings::GetInstance().SetWindowSize({ 16 * 25 * 2, 16 * 25 * 2 });
    Mage::GameSettings::GetInstance().SetWindowTitle("Dungeon Generator");

    // Run
    Game game;
    game.Run();

    return 0;
}
