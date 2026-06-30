#pragma once
#include <string>
#include <vector>
#include "Handle/Handle.h"
#include "Camera2DResource/Camera2DResource.h"

#include "DebugCamera2DResource/DebugCamera2DResource.h"

#include <unordered_map>

namespace Engine
{
	class Camera2DStore
	{
	public:

		/// @brief コンストラクタ
		Camera2DStore();

		/// @brief 読み込み
		/// @param name 
		/// @return 
		Camera2DHandle Load(const std::string& name);

		/// @brief 更新処理
		void Update(bool isHoverViewWindow);

		/// @brief カメラを切り替える
		/// @param hCamera 
		void Switch(Camera2DHandle hCamera) { selectHCamera_ = hCamera; }

		/// @brief カメラを切り替える
		/// @param name 
		void Switch(const std::string& name) { selectHCamera_ = nameTable_[name]; }

		/// @brief 2Dカメラデータを取得する
		/// @return 
		const Camera2D& GetCamera2D() const;

		/// @brief パラメータを取得する
		/// @param hCamera 
		/// @return 
		Camera2DData::Param* GetParam(Camera2DHandle hCamera) { return dataTable_[hCamera]->GetCamera2D().GetParam(); }

		/// @brief パラメータを取得する
		/// @param name 
		/// @return 
		Camera2DData::Param* GetParam(const std::string& name) { return dataTable_[nameTable_[name]]->GetCamera2D().GetParam(); }


	private:

		/// @brief 初期読み込み
		/// @param name 
		/// @return 
		Camera2DHandle InitialLoad(const std::string& name);

		// 選択中のカメラハンドル
		Camera2DHandle selectHCamera_ = 0;


	private:

		// データテーブル
		std::vector<std::unique_ptr<Camera2DResource>> dataTable_;

		/// @brief 名前テーブル
		std::unordered_map<std::string, Camera2DHandle> nameTable_;


	private:

#ifdef _DEVELOPMENT

		// デバッグカメラ
		std::unique_ptr<DebugCamera2DResource> debugCamera_ = nullptr;

#endif
	};
}