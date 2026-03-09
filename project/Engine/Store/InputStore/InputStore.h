#pragma once
#include "InputResource/InputBaseResource.h"

#include <vector>
#include <unordered_map>

namespace Engine
{
	class InputStore
	{
	public:

		/// @brief 初期化
		/// @param input 
		void Initialize(Input* input);

		/// @brief 更新処理
		void Update();

		/// @brief キー読み込み
		/// @param name 
		/// @param inputState 
		/// @param key 
		/// @return 
		InputHandle LoadKey(const std::string& name, InputState inputState, BYTE key);

		/// @brief ゲームパッドボタン読み込み
		/// @param name 
		/// @param inputState 
		/// @param controller 
		/// @param button 
		/// @return 
		InputHandle LoadGamepadButton(const std::string& name, InputState inputState, DWORD controller, DWORD button);

		/// @brief ゲームパッドスティック読み込み
		/// @param name 
		/// @param inputState 
		/// @param stickType 
		/// @param controller 
		/// @param direction 
		/// @param dot 
		/// @return 
		InputHandle LoadGamepadStick(const std::string& name, InputState inputState, StickType stickType, DWORD controller, const Vector2& direction, float dot);

		/// @brief ゲームパッドトリガー読み込み
		/// @param name 
		/// @param inputState 
		/// @param triggerType 
		/// @param controller 
		/// @param threshold 
		/// @return 
		InputHandle LoadGamepadTrigger(const std::string& name, InputState inputState, TriggerType triggerType, DWORD controller, float threshold);


		/// @brief パラメータを取得する
		/// @tparam T 
		/// @param hInput 
		/// @return 
		template<typename T>
		T* GetParam(InputHandle hInput)
		{
			return static_cast<T*>(dataTable_[hInput]->GetParam());
		}

		/// @brief パラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetParam(const std::string& name)
		{
			return static_cast<T*>(dataTable_[nameTable_[name]]->GetParam());
		}

		/// @brief 入力したかどうか
		/// @param hInput 
		/// @return 
		bool IsInput(InputHandle hInput) { return dataTable_[hInput]->IsInput(); }

		/// @brief 入力したかどうか
		/// @param name 
		/// @return 
		bool IsInput(const std::string& name) { return dataTable_[nameTable_[name]]->IsInput(); }


	private:

		// データテーブル
		std::vector<std::unique_ptr<InputBaseResource>> dataTable_;

		// 名前テーブル
		std::unordered_map<std::string, InputHandle> nameTable_;


		/// @brief 入力処理
		Input* input_ = nullptr;
	};
}