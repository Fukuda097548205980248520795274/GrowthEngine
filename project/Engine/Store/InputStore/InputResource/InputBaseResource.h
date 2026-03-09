#pragma once
#include "Data/InputData/InputData.h"
#include "Handle/Handle.h"
#include <string>
#include <memory>

/// @brief 入力状態
enum class InputState
{
	Press,
	Trigger,
	Release
};

/// @brief スティックの種類
enum class StickType
{
	Left,
	Right
};

/// @brief トリガーの種類
enum class TriggerType
{
	Left,
	Right
};

namespace Engine
{
	class InputBaseResource
	{
	public:

		/// @brief 仮想デストラクタ
		virtual ~InputBaseResource() = default;

		/// @brief コンストラクタ
		/// @param inputState 
		/// @param hInput 
		/// @param name 
		InputBaseResource(InputState inputState, InputHandle hInput, const std::string& name)
			: inputState_(inputState), hInput_(hInput), name_(name) {
		};

		/// @brief 初期化
		/// @param input 
		void Initialize(Input* input);

		/// @brief 更新処理
		virtual void Update() = 0;

		/// @brief ハンドルを取得する
		/// @return 
		InputHandle GetHandle()const { return hInput_; }

		/// @brief 入力状態を取得する
		/// @return 
		InputState GetKeyState()const { return inputState_; }

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }

		/// @brief 入力されたかどうか
		/// @return 
		bool IsInput()const { return isInput_; }

		/// @brief パラメータを取得する
		/// @return 
		virtual void* GetParam() = 0;


	protected:

		/// @brief 入力状態
		InputState inputState_;

		/// @brief 入力
		Input* input_ = nullptr;

		// 入力されたかどうか
		bool isInput_ = false;


	private:

		/// @brief 名前
		std::string name_{};

		// ハンドル
		InputHandle hInput_ = 0;
	};
}