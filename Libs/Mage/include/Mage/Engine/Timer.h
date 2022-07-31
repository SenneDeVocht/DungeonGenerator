#pragma once
#include "../Singleton.h"

namespace Mage
{
	class Timer final : public Singleton<Timer>
	{
	public:
		~Timer();

		void CalculateTime() const;

		float GetDeltaTime() const;
		float GetFixedTimeStep() const;

	private:
		friend class Singleton<Timer>;
		Timer();

		class TimerImpl;
		std::unique_ptr<TimerImpl> m_pImpl;
	};
}