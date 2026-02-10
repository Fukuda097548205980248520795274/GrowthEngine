#pragma once
#include <string>
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

namespace Engine
{
	class PrimitiveBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		PrimitiveBaseData(const std::string& name) : name_(name){}

		/// @brief 仮想デストラクタ
		virtual ~PrimitiveBaseData() = default;

		/// @brief 更新処理
		/// @param viewProjection 
		virtual void Update(const Matrix4x4& viewProjection) = 0;

		/// @brief シャドウマップ用の更新処理
		/// @param viewProjection 
		virtual void ShadowMapUpdate(const Matrix4x4& viewProjection) = 0;

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }
		

	protected:

		// 名前
		std::string name_{};

		// 種別名
		std::string typeName_{};
	};
}