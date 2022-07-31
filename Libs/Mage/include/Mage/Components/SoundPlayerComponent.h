#pragma once
#include "Mage/Components/Component.h"

namespace Mage
{
	class SoundInfo;

	class SoundPlayerComponent : public Component
	{
	public:
		SoundPlayerComponent(std::shared_ptr<SoundInfo> soundInfo);
		~SoundPlayerComponent();

		void Play() const;
		void Stop() const;

		void SetVolume(float volume) const;
		float GetVolume() const;

		void SetLoop(bool loop) const;
		bool GetLoop() const;

	private:
		std::shared_ptr<SoundInfo> m_pSoundInfo;
	};
}
