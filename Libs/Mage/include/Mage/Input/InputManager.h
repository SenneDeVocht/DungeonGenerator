#pragma once

namespace Mage
{
	class Command;

	enum class InputState
	{
		Down,
		Hold,
		Up
	};

	enum class ControllerButton
	{
		A,
		B,
		X,
		Y,
		DPadUp,
		DPadDown,
		DPadLeft,
		DPadRight,
		LeftShoulder,
		RightShoulder,
		RightStick,
		LeftStick,
		Start,
		Back
	};

	enum class ControllerAxis
	{
		LeftStickX,
		LeftStickY,
		RightStickX,
		RightStickY,
		LeftTrigger,
		RightTrigger
	};

	struct InputAction
	{
		int ControllerIndex;
		ControllerButton Button;

		int KeyboardKey;

		InputState State;
		std::unique_ptr<Command> Command;
	};

	class InputManager
	{
	public:
		virtual ~InputManager() = default;

		virtual bool ProcessInput() const = 0;
		virtual void AddInputAction(InputAction* action) const = 0;
		virtual void RemoveInputAction(InputAction* action) const = 0;

		virtual bool IsControllerConnected(int controllerIndex) const = 0;
	    virtual bool CheckControllerButton(int controllerIndex, ControllerButton button, InputState state) const = 0;
		virtual float GetControllerAxis(int controllerIndex, ControllerAxis axis) const = 0;

		virtual bool CheckKeyboardKey(int keyboardKey, InputState state) const = 0;
	};

	class NullInputManager final : public InputManager
	{
	public:
		bool ProcessInput() const override { return true; }
		void AddInputAction(InputAction*) const override {}
		void RemoveInputAction(InputAction*) const override {}

		bool IsControllerConnected(int) const override { return false; }
		bool CheckControllerButton(int, ControllerButton, InputState) const override { return false; }
		float GetControllerAxis(int, ControllerAxis) const override { return 0.0f; }

		bool CheckKeyboardKey(int, InputState) const override { return false; }
	};

	class XInputInputManager final : public InputManager
	{
	public:
		XInputInputManager();
		~XInputInputManager() override;
		
        XInputInputManager(const XInputInputManager& other) = delete;
        XInputInputManager(XInputInputManager&& other) noexcept = delete;
        XInputInputManager& operator=(const XInputInputManager& other) = delete;
        XInputInputManager& operator=(XInputInputManager&& other) noexcept = delete;

		bool ProcessInput() const override;
		void AddInputAction(InputAction* action) const override;
		void RemoveInputAction(InputAction* action) const override;

		bool IsControllerConnected(int controllerIndex) const override;
		bool CheckControllerButton(int controllerIndex, ControllerButton button, InputState state) const override;
		float GetControllerAxis(int controllerIndex, ControllerAxis axis) const override;

		bool CheckKeyboardKey(int keyboardKey, InputState state) const override;

	private:
		class XInputInputManagerImpl;
		std::unique_ptr<XInputInputManagerImpl> m_pImpl;
	};
}
