#pragma once
#include <string>
#include <vector>
#include "Handle/Handle.h"
#include "CameraResource/CameraResource.h"
#include "DebugCameraResource/DebugCameraResource.h"

class GameCamera;

namespace Engine
{

	class CameraStore
	{
	public:

		/// @brief コンストラクタ
		CameraStore();

		/// @brief 読み込み
		/// @param gameCamera 
		/// @param name 
		/// @return 
		CameraHandle Load(GameCamera* gameCamera, const std::string& name);

		/// @brief 更新処理
		void Update();

		/// @brief カメラを切り替える
		/// @param hCamera 
		void Switch(CameraHandle hCamera) { selectHCamera_ = hCamera; }

		/// @brief 3Dカメラデータを取得する
		/// @return 
		const Camera3D& GetCamera3D() const;


	private:

		/// @brief 初期読み込み
		/// @param name 
		/// @return 
		CameraHandle InitialLoad(const std::string& name);

		// 選択中のカメラハンドル
		CameraHandle selectHCamera_ = 0;


	private:

		// データテーブル
		std::vector<std::unique_ptr<CameraResource>> dataTable_;


	private:

#ifdef _DEVELOPMENT

		// デバッグカメラ
		std::unique_ptr<DebugCameraResource> debugCamera_ = nullptr;

#endif
	};
}