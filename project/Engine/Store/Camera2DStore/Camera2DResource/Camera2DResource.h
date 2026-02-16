#pragma once
#include <memory>
#include <string>
#include "Camera/Camera2D/Camera2D.h"
#include "Handle/Handle.h"

namespace Engine
{
	class Camera2DResource
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param hCamera 
		Camera2DResource(const std::string& name, Camera2DHandle hCamera);

		/// @brief 更新処理
		void Update();

		/// @brief カメラデータを取得する
		/// @return 
		const Camera2D& GetCamera2D() { return *camera2d_; }

		/// @brief ハンドルを取得する
		/// @return 
		Camera2DHandle GetHandle() { return hCamera_; }

		/// @brief 名前を取得する
		/// @return 
		std::string GetName() { return name_; }


	private:

		// カメラ2D
		std::unique_ptr<Camera2D> camera2d_ = nullptr;

		// カメラハンドル
		Camera2DHandle hCamera_ = 0;

		// 名前
		std::string name_{};
	};
}