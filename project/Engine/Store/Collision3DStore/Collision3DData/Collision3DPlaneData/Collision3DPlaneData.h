#pragma once
#include "../Collision3DBaseData.h"

namespace Engine
{
	class Collision3DPlaneData : public Collision3DBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param func 
		Collision3DPlaneData(std::function<void()> func, const std::string& name, Collision3D::Type type, Collision3DHandle hCollision);

		/// @brief インスタンスを作成する
		/// @return 
		void* CreateInstance() override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() override { return param_.get(); }


	private:

		// パラメータ
		std::unique_ptr<Collision3D::Plane> param_ = nullptr;

		/// @brief インスタンスのパラメータ
		std::list<std::unique_ptr<Collision3D::Plane>> instanceParams_;
	};
}