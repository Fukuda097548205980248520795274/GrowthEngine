#pragma once
#include <string>
#include "Handle/Handle.h"
#include "Data/Render3DData/Render3DData.h"

class GrowthEngine;
class WorldTransform3D;

namespace Engine
{
	class BaseRender3D
	{
	public:

		/// @brief 仮想デストラクタ
		virtual ~BaseRender3D() = default;

		/// @brief コンストラクタ
		/// @param name 
		BaseRender3D(const std::string& name);

		/// @brief 親を設定する
		/// @param parent 
		void SetParent(WorldTransform3D* parent);

		/// @brief 描画処理
		virtual void Draw() = 0;


	protected:


		// エンジン
		const GrowthEngine* engine_ = nullptr;

		// 名前
		std::string name_{};

		// 種類
		Engine::Render3D::Type type_;

		// 3D描画ハンドル
		Render3DHandle hRender3D_ = 0;
	};
}