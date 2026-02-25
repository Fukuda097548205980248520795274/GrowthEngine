#pragma once
#include "../CollisionBase3DData.h"

namespace Engine
{
	class CollisionOBBData : public CollisionBase3DData
	{
	public:

		/// @brief コンストラクタ
		/// @param func 
		CollisionOBBData(std::function<void()> func, const std::string& name, Collision3D::Type type, Collision3DHandle hCollision);

		/// @brief インスタンスを作成する
		/// @return 
		void* CreateInstance() override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() override { return param_.get(); }


	private:

		// パラメータ
		std::unique_ptr<Collision3D::OBB> param_ = nullptr;

		/// @brief インスタンスのパラメータ
		std::list<std::unique_ptr<Collision3D::OBB>> instanceParams_;
	};
}