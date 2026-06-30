#pragma once
#include <string>
#include <vector>
#include "Handle/Handle.h"
#include "Camera3DResource/Camera3DResource.h"
#include "DebugCamera3DResource/DebugCamera3DResource.h"
#include "Parameter/Camera3DParameter/Camera3DParameter.h"
#include "DataForGPU/CameraDataForGPU/CameraDataForGPU.h"

#include <unordered_map>

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

		/// @brief シーン前のリセット
		void PerSceneReset();

		/// @brief 読み込み
		/// @param name 
		/// @return 
		Camera3DHandle Load(const std::string& name);

		/// @brief 更新処理
		void Update(bool isHoverViewWindow);

		/// @brief カメラを切り替える
		/// @param hCamera 
		void Switch(Camera3DHandle hCamera) { selectHCamera_ = hCamera; }

		/// @brief カメラを切り替える
		/// @param name 
		void Switch(const std::string& name) { selectHCamera_ = nameTable_[name]; }

		/// @brief 3Dカメラデータを取得する
		/// @return 
		const Camera3D& GetCamera3D() const;

		/// @brief パメータを取得する
		/// @param hCamera 
		/// @return 
		Camera3DData::Param* GetParam(Camera3DHandle hCamera) { return dataTable_[hCamera]->GetCamera3D().GetParam(); }

		/// @brief パラメータを取得する
		/// @param name 
		/// @return 
		Camera3DData::Param* GetParam(const std::string& name) { return dataTable_[nameTable_[name]]->GetCamera3D().GetParam(); }

		/// @brief 選択中のカメラをパラメータを取得する
		/// @return 
		Camera3DData::Param* GetSelectParam();



		/// @brief カメラリソースをコマンドリストに登録
		/// @param commandList 
		/// @param rootParameterIndex 
		void RegisterCameraResource(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)const { cameraResource_->RegisterGraphics(commandList, rootParameterIndex); }

		/// @brief ジッタリングの設定
		/// @param enable 
		void SetEnableJitter(bool enable) { enableJitter_ = enable; }


	public:

		/// @brief デバッグ用の線を描画する
		void DebugDrawLine();

		/// @brief デバッグ用パラメータ
		void DebugParameter();

		/// @brief デバッグ用レイピッキング
		/// @param ray 
		/// @param pickList 
		void DebugRayPicking(const Collision3D::Ray& ray, std::vector<std::pair<float, DebugData::DebugGuizmoData*>>& pickList);


	private:

		/// @brief 初期読み込み
		/// @param name 
		/// @return 
		Camera3DHandle InitialLoad(const std::string& name);

		// 選択中のカメラハンドル
		Camera3DHandle selectHCamera_ = 0;

		/// @brief カメラリソース
		std::unique_ptr<ConstantBufferResource<CameraDataForGPU>> cameraResource_ = nullptr;

		/// @brief パラメータ
		std::unique_ptr<Camera3DParameter> parameter_ = nullptr;

		/// @brief ジッタリングを有効にするかどうか
		bool enableJitter_ = false;


	private:

		// データテーブル
		std::vector<std::unique_ptr<Camera3DResource>> dataTable_;

		/// @brief 名前テーブル
		std::unordered_map<std::string, Camera3DHandle> nameTable_;


	private:

#ifdef _DEVELOPMENT

		// デバッグカメラ
		std::unique_ptr<DebugCamera3DResource> debugCamera_ = nullptr;

#endif
	};
}