#pragma once
#include "../InputBaseResource.h"

namespace Engine
{
	class InputGamepadButtonResource : public InputBaseResource
	{
	public:

		/// @brief コンストラクタ
		/// @param inputState 
		/// @param hInput 
		/// @param name 
		/// @param controller 
		/// @param button 
		InputGamepadButtonResource(InputState inputState, InputHandle hInput, const std::string& name, DWORD controller, DWORD button);

		/// @brief 更新処理
		void Update() override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() override { return param_.get(); }


	private:

		/// @brief パラメータ
		std::unique_ptr<InputData::GamepadButtonParam> param_ = nullptr;
	};
}