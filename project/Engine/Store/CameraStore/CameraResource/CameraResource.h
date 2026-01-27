#pragma once
#include <memory>
#include <string>
#include "Application/Camera/Camera3D/Camera3D.h"
#include "Handle/Handle.h"

class GameCamera;

namespace Engine
{
	class CameraResource
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param hCamera 
		CameraResource(const std::string& name, CameraHandle hCamera);

		/// @brief 更新処理
		void Update();

		/// @brief カメラデータを取得する
		/// @return 
		const Camera3D& GetCamera3D() { return *camera3d_; }
		
		/// @brief ハンドルを取得する
		/// @return 
		CameraHandle GetHandle() { return hCamera_; }

		/// @brief 名前を取得する
		/// @return 
		std::string GetName() { return name_; }

		/// @brief データを反映させる
		/// @param gameCamera 
		void DataReflect(GameCamera* gameCamera);


	private:

		// カメラ3D
		std::unique_ptr<Camera3D> camera3d_ = nullptr;

		// カメラハンドル
		CameraHandle hCamera_ = 0;

		// 名前
		std::string name_{};
	};
}