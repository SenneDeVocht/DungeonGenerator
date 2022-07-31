#pragma once

struct SDL_Window;

namespace Mage
{
	class MageGame
	{
	public:
		MageGame() = default;
		virtual ~MageGame();

		MageGame(const MageGame& other) = delete;
		MageGame(MageGame&& other) = delete;
		MageGame& operator=(const MageGame& other) = delete;
		MageGame& operator=(MageGame&& other) = delete;


		void Initialize();
		virtual void LoadGame() const = 0;
		void Cleanup();
		void Run();

	private:
		SDL_Window* m_pWindow{};
	};
}

