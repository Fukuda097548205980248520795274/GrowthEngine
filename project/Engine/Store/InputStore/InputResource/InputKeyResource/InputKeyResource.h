#pragma once
#include "../InputBaseResource.h"

namespace Engine
{
	class InputKeyResource : public InputBaseResource
	{
	public:

		/// @brief コンストラクタ
		/// @param inputState 
		/// @param hInput 
		/// @param name 
		InputKeyResource(InputState inputState, InputHandle hInput, const std::string& name, BYTE key);

		/// @brief 更新処理
		void Update() override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() override { return param_.get(); }


	private:

		/// @brief パラメータ
		std::unique_ptr<InputData::KeyParam> param_ = nullptr;
	};
}