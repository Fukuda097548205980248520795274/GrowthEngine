#pragma once
#include "../Collision3DBaseData.h"

class GrowthEngine;

namespace Engine
{
	class Collision3DCapsuleData : public Collision3DBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param func 
		Collision3DCapsuleData(const std::string& name, Collision3D::Type type, Collision3DHandle hCollision);

		/// @brief インスタンスを作成する
		/// @return 
		void* CreateInstance() override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() override { return param_.get(); }


	public:

		/// @brief デバッグ用の線を描画する
		void DebugDrawLine() override;


	private:

		/// @brief エンジン
		const GrowthEngine* engine_ = nullptr;

		// パラメータ
		std::unique_ptr<Collision3D::Capsule> param_ = nullptr;
	};
}