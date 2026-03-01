#pragma once
#include <string>
#include <vector>
#include "Handle/Handle.h"
#include "Camera3DResource/Camera3DResource.h"
#include "DebugCameraResource/DebugCameraResource.h"

#include "Resource/ConstantBufferResource/ConstantBufferResource.h"

namespace Engine
{
	class Log;

	class Camera3DStore
	{
	public:

		/// @brief コンストラクタ
		Camera3DStore();

		/// @brief 初期化
		/// @param device 
		/// @param log 
		void Initialize(ID3D12Device* device, Log* log);

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

		/// @brief パメータを取得する
		/// @param hCamera 
		/// @return 
		Camera3DData::Param* GetParam(Camera3DHandle hCamera) { return dataTable_[hCamera]->GetCamera3D().GetParam(); }

		/// @brief カメラリソースをコマンドリストに登録
		/// @param commandList 
		/// @param rootParameterIndex 
		void RegisterCameraResource(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex) { cameraResource_->RegisterGraphics(commandList, rootParameterIndex); }


	private:

		/// @brief 初期読み込み
		/// @param name 
		/// @return 
		Camera3DHandle InitialLoad(const std::string& name);

		// 選択中のカメラハンドル
		Camera3DHandle selectHCamera_ = 0;

		/// @brief カメラリソース
		std::unique_ptr<ConstantBufferResource<Vector4>> cameraResource_ = nullptr;


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