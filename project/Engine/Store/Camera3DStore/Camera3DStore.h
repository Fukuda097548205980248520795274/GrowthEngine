#pragma once
#include <string>
#include <vector>
#include "Handle/Handle.h"
#include "Camera3DResource/Camera3DResource.h"
#include "DebugCameraResource/DebugCameraResource.h"

namespace Engine
{

	class Camera3DStore
	{
	public:

		/// @brief コンストラクタ
		Camera3DStore();

		/// @brief 読み込み
		/// @param name 
		/// @return 
		Camera3DHandle Load(const std::string& name);

		/// @brief 更新処理
		void Update();

		/// @brief カメラを切り替える
		/// @param hCamera 
		void Switch(Camera3DHandle hCamera) { selectHCamera_ = hCamera; }

		/// @brief 3Dカメラデータを取得する
		/// @return 
		const Camera3D& GetCamera3D() const;


	private:

		/// @brief 初期読み込み
		/// @param name 
		/// @return 
		Camera3DHandle InitialLoad(const std::string& name);

		// 選択中のカメラハンドル
		Camera3DHandle selectHCamera_ = 0;


	private:

		// データテーブル
		std::vector<std::unique_ptr<Camera3DResource>> dataTable_;


	private:

#ifdef _DEVELOPMENT

		// デバッグカメラ
		std::unique_ptr<DebugCameraResource> debugCamera_ = nullptr;

#endif
	};
}