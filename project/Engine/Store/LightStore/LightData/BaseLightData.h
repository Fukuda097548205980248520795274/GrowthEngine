#pragma once
#include <string>
#include <memory>
#include "Handle/Handle.h"
#include "Math/Vector/Vector3/Vector3.h"
#include "Data/LightData/LightData.h"

namespace Engine
{
	class LightParameter;

	class BaseLightData
	{
	public:

		/// @brief 仮想デストラクタ
		virtual ~BaseLightData() = default;

		/// @brief 初期化
		/// @param parameter 
		virtual void Initialize(LightParameter* parameter);

		/// @brief リセット
		virtual void Reset() = 0;

		/// @brief シーン前処理
		void PerScene() { isUse_ = false; }

		/// @brief コンストラクタ
		/// @param name 
		/// @param handle 
		BaseLightData(const std::string& name, LightHandle handle) : name_(name), handle_(handle) { isUse_ = true; }

		/// @brief 輝度を取得する
		/// @return 
		virtual float GetIntensity() = 0;

		/// @brief 名前を取得する
		/// @return 
		const char* GetName() const { return name_.c_str(); }

		/// @brief ハンドルを取得する
		/// @return 
		LightHandle GetHandle()const { return handle_; }

		/// @brief 
		/// @return 
		virtual void* GetParam() = 0;

		/// @brief 種類を取得する
		/// @return 
		virtual Light::Type GetType() const = 0;

		/// @brief デバッグ用描画処理
		virtual void DebugDraw() = 0;

		/// @brief 使用しているかどうか
		/// @return 
		bool IsUse()const { return isUse_; }


	protected:

		// 名前
		std::string name_{};

		/// @brief グループ名
		std::string group_{};

		// ハンドル
		LightHandle handle_ = 0;

		/// @brief パラメータ
		LightParameter* parameter_ = nullptr;

		// 使用フラグ
		bool isUse_ = false;
	};
}