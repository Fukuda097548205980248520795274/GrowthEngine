#pragma once
#include "../Collision2DBaseData.h"

namespace Engine
{
	class Collision2DSpriteData : public Collision2DBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param func 
		Collision2DSpriteData(std::function<void()> func, const std::string& name, Collision2D::Type type, Collision2DHandle hCollision);

		/// @brief インスタンスを作成する
		/// @return 
		void* CreateInstance() override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() override { return param_.get(); }


	private:

		// パラメータ
		std::unique_ptr<Collision2D::Sprite> param_ = nullptr;
	};
}