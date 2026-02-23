#pragma once
#include <string>
#include <vector>
#include "Handle/Handle.h"
#include "Camera2DResource/Camera2DResource.h"

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
		void Update();

		/// @brief カメラを切り替える
		/// @param hCamera 
		void Switch(Camera2DHandle hCamera) { selectHCamera_ = hCamera; }

		/// @brief 2Dカメラデータを取得する
		/// @return 
		const Camera2D& GetCamera2D() const;

		/// @brief パラメータを取得する
		/// @param hCamera 
		/// @return 
		Camera2DData::Param* GetParam(Camera2DHandle hCamera)const { return dataTable_[hCamera]->GetCamera2D().GetParam(); }


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
	};
}