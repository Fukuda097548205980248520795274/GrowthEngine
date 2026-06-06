#pragma once
#include "../InputBaseResource.h"

namespace Engine
{
	class InputMouseResource : public InputBaseResource
	{
	public:

		/// @brief コンストラクタ
		/// @param button 
		/// @param inputState 
		/// @param hInput 
		/// @param name 
		InputMouseResource(MouseButton button, InputState inputState, InputHandle hInput, const std::string& name);

		/// @brief 更新処理
		void Update() override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() override { return param_.get(); }


	private:

		/// @brief パラメータ
		std::unique_ptr<InputData::MouseButtonParam> param_ = nullptr;
	};
}