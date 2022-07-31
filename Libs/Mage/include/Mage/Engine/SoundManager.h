#pragma once

namespace Mage
{
	class SoundInfo;

	class SoundManager
	{
	public:
		SoundManager() = default;
        virtual ~SoundManager() = default;

        SoundManager(const SoundManager& other) = delete;
        SoundManager(SoundManager&& other) noexcept = delete;
        SoundManager& operator=(const SoundManager& other) = delete;
        SoundManager& operator=(SoundManager&& other) noexcept = delete;

		virtual void Update() = 0;
		virtual void LoadSound(SoundInfo* soundInfo) = 0;
		virtual void PlaySound(SoundInfo* soundInfo) = 0;
		virtual void StopSound(SoundInfo* soundInfo) = 0;
	};

	class NullSoundManager final : public SoundManager
	{
	public:
		void Update() override {}
		void LoadSound(SoundInfo*) override {}
		void PlaySound(SoundInfo*) override {}
		void StopSound(SoundInfo*) override {}
	};

	class FMODSoundManager final : public SoundManager
	{
	public:
		FMODSoundManager();
		~FMODSoundManager() override;

		FMODSoundManager(const FMODSoundManager& other) = delete;
		FMODSoundManager(FMODSoundManager&& other) noexcept = delete;
		FMODSoundManager& operator=(const FMODSoundManager& other) = delete;
		FMODSoundManager& operator=(FMODSoundManager&& other) noexcept = delete;

		void Update() override;
		void LoadSound(SoundInfo* pSoundInfo) override;
		void PlaySound(SoundInfo* pSoundInfo) override;
		void StopSound(SoundInfo* pSoundInfo) override;

	private:
		class FMODSoundManagerImpl;
		std::unique_ptr<FMODSoundManagerImpl> m_pImpl;
	};
}
