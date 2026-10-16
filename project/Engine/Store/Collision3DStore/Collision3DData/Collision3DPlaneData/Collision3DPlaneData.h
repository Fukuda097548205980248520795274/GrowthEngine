#pragma once
#include "../Collision3DBaseData.h"

namespace Engine
{
	/// @brief 3D衝突判定の平面データ
	class Collision3DPlaneData : public Collision3DBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param func 
		Collision3DPlaneData(const std::string& name, Collision3D::Type type, Collision3DHandle hCollision);

		/// @brief インスタンスを作成する
		/// @return 
		void* CreateInstance() override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() override { return param_.get(); }

		/// @brief デバッグ用の線を描画する
		void DebugDrawLine() override;


	private:

		// パラメータ
		std::unique_ptr<Collision3D::Plane> param_ = nullptr;
	};
}