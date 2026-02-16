#pragma once
#include <memory>
#include <string>
#include "Camera/Camera3D/Camera3D.h"
#include "Handle/Handle.h"

namespace Engine
{
	class Camera3DResource
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param hCamera 
		Camera3DResource(const std::string& name, Camera3DHandle hCamera);

		/// @brief 更新処理
		void Update();

		/// @brief カメラデータを取得する
		/// @return 
		const Camera3D& GetCamera3D() { return *camera3d_; }
		
		/// @brief ハンドルを取得する
		/// @return 
		Camera3DHandle GetHandle() { return hCamera_; }

		/// @brief 名前を取得する
		/// @return 
		std::string GetName() { return name_; }


	private:

		// カメラ3D
		std::unique_ptr<Camera3D> camera3d_ = nullptr;

		// カメラハンドル
		Camera3DHandle hCamera_ = 0;

		// 名前
		std::string name_{};
	};
}