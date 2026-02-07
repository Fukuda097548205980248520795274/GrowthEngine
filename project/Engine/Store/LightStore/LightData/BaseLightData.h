#pragma once
#include <string>
#include <memory>
#include "Handle/Handle.h"
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Vector/Vector4/Vector4.h"

namespace Engine
{
	/// @brief 平行光源パラメータ
	struct DirectionalLightParam
	{
		// 向き
		std::unique_ptr<Vector3> direction = nullptr;

		// 輝度
		std::unique_ptr<float> intensity = nullptr;

		// 色
		std::unique_ptr<Vector4> color = nullptr;
	};

	class BaseLightData
	{
	public:

		/// @brief 仮想デストラクタ
		virtual ~BaseLightData() = default;

		/// @brief コンストラクタ
		/// @param name 
		/// @param handle 
		BaseLightData(const std::string& name, LightHandle handle) : name_(name), handle_(handle){}

		/// @brief 名前を取得する
		/// @return 
		const char* GetName() const { return name_.c_str(); }

		/// @brief ハンドルを取得する
		/// @return 
		LightHandle GetHandle()const { return handle_; }

		/// @brief 
		/// @return 
		virtual void* GetParam() = 0;

		/// @brief 種別名を取得する
		/// @return 
		virtual const char* GetTypeName() const = 0;


	protected:

		// 名前
		std::string name_{};

		// ハンドル
		LightHandle handle_ = 0;
	};
}