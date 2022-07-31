#pragma once
#include "Mage/Components/Component.h"

namespace Mage
{
	class Texture2D;

	struct SpriteAnimation
	{
		SpriteAnimation(const std::shared_ptr<Texture2D>& pSpritesheet, int numFrames, float secondsPerFrame, bool loop = true)
			: pSpritesheet{ pSpritesheet }
			, NumFrames{ numFrames }
			, SecondsPerFrame{ secondsPerFrame }
			, Loop{ loop }
		{}

		std::shared_ptr<Texture2D> pSpritesheet;
		int NumFrames;
		float SecondsPerFrame;
		bool Loop;
	};

	class AnimatedSpriteComponent : public Component
	{
	public:
		explicit AnimatedSpriteComponent(float renderLayer = 0.0f);

		void Update() override;
		void DrawProperties() override;
		void Render() const override;

		void SetAnimation(const std::shared_ptr<SpriteAnimation>& animation);
		SpriteAnimation* GetAnimation() const { return m_pAnimation.get(); }

	private:
		std::shared_ptr<SpriteAnimation> m_pAnimation{};

		int m_CurrentFrame{ 0 };
		float m_Timer{ 0 };

		float m_RenderLayer;
	};
}