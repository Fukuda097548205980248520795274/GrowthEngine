#pragma once
#include "../InputBaseResource.h"

namespace Engine
{
	class InputGamepadLeftTriggerResource : public InputBaseResource
	{
	public:

		/// @brief コンストラクタ
		/// @param inputState 
		/// @param hInput 
		/// @param name 
		/// @param controller 
		/// @param threshold 
		InputGamepadLeftTriggerResource(InputState inputState, InputHandle hInput, const std::string& name, DWORD controller, float threshold);

		/// @brief 更新処理
		void Update() override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() override { return param_.get(); }


	private:

		/// @brief パラメータ
		std::unique_ptr<InputData::GamepadTriggerParam> param_ = nullptr;

		/// @brief 入力されたかどうかを記録
		bool isRecordInput_ = false;

		/// @brief 前回の入力が記録されたかどうかを記録
		bool isPrevRecordInput_ = false;
	};
}