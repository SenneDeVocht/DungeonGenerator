#pragma once
#include "Mage/Engine/MageGame.h"

class Game final : public Mage::MageGame
{
public:
	void LoadGame() const override;
};
