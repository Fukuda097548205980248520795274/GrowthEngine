#include "Camera3DStore.h"
#include <cassert>
#include "RenderContext/ImGuiRender/ImGuiRender.h"

/// @brief コンストラクタ
Engine::Camera3DStore::Camera3DStore()
{
	// パラメータの生成
	parameter_ = std::make_unique<Camera3DParameter>("Camera3D");

	// 初期カメラを読み込む
	selectHCamera_ = InitialLoad("Initial");

#ifdef DEVELOPMENT

	// デバッグカメラの生成
	debugCamera_ = std::make_unique<DebugCamera3DResource>();

#endif
}

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::Camera3DStore::Initialize(ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(device);

	// カメラリソースの生成と初期化
	cameraResource_ = std::make_unique<ConstantBufferResource<CameraDataForGPU>>();
	cameraResource_->Initialize(device, log);
}

/// @brief シーン前のリセット
void Engine::Camera3DStore::PerSceneReset()
{
	for (auto& data : dataTable_)data->PerSceneReset();

	// ジッタリング無効
	enableJitter_ = false;
}

/// @brief 読み込み
/// @param name 名前
/// @return 
Camera3DHandle Engine::Camera3DStore::Load(const std::string& name)
{
	// 同じデータがないか確認
	for (auto& data : dataTable_)
	{
		if (name == data->GetName())
		{
			// リセットしてハンドルを返す
			data->Reset();
			return data->GetHandle();
		}
	}

	// ハンドルの値
	Camera3DHandle hCamera = static_cast<Camera3DHandle>(dataTable_.size());

	// 名前テーブルに記録する
	nameTable_[name] = hCamera;

	// 初めての読み込みカメラは自動で切り替え
	if (dataTable_.size() == 1)selectHCamera_ = hCamera;

	// カメラリソースの生成
	std::unique_ptr<Camera3DResource> data = std::make_unique<Camera3DResource>(name, hCamera, parameter_.get());
	dataTable_.push_back(std::move(data));

	return hCamera;
}

/// @brief 更新処理
void Engine::Camera3DStore::Update(bool isHoverViewWindow)
{
	// 指定されたカメラの更新
	if(!enableJitter_)dataTable_[selectHCamera_]->Update();
	else dataTable_[selectHCamera_]->JitterUpdate();

	Vector3 cameraPosition = dataTable_[selectHCamera_]->GetCamera3D().GetWorldPosition();

#ifdef DEVELOPMENT

	// デバッグカメラ更新
	if(!enableJitter_)debugCamera_->Update(isHoverViewWindow);
	else debugCamera_->JitterUpdate(isHoverViewWindow);

	// デバッグカメラ有効時
	if (debugCamera_->IsEnable())
		cameraPosition = debugCamera_->GetCamera3D().GetWorldPosition();

#endif

	// 指定されたカメラのデータ
	cameraResource_->data_->position = cameraPosition;
	cameraResource_->data_->nearZ = dataTable_[selectHCamera_]->GetCamera3D().GetParam()->setting.nearClip;
	cameraResource_->data_->farZ = dataTable_[selectHCamera_]->GetCamera3D().GetParam()->setting.farClip;
}

/// @brief 3Dカメラデータを取得する
/// @return 
const Engine::Camera3D& Engine::Camera3DStore::GetCamera3D() const
{
#ifdef DEVELOPMENT

	// デバッグカメラ有効時
	if (debugCamera_->IsEnable())
		return debugCamera_->GetCamera3D();

#endif

	return dataTable_[selectHCamera_]->GetCamera3D(); 
}

Engine::Camera3DData::Param* Engine::Camera3DStore::GetSelectParam()
{
#ifdef DEVELOPMENT

	// デバッグカメラ有効時
	if(debugCamera_->IsEnable())
		return debugCamera_->GetCamera3D().GetParam();

#endif

	return dataTable_[selectHCamera_]->GetCamera3D().GetParam(); 
}


/// @brief デバッグ用の線を描画する
/// @param color 
void Engine::Camera3DStore::DebugDrawLine()
{
	for (auto& data : dataTable_)
	{
		// 選択中のカメラ
		if (data->GetHandle() == selectHCamera_)
		{
			data->DebugDrawLine(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		}
		else
		{
			// 未選択のカメラ
			data->DebugDrawLine(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		}

#ifdef DEVELOPMENT

		// デバッグカメラ使用中
		if (debugCamera_->IsEnable())
		{
			data->DebugGuizmo(debugCamera_->GetCamera3D().GetViewMatrix(), debugCamera_->GetCamera3D().GetProjectionMatrix());
		}
		else
		{
			// デバッグカメラ未使用中
			data->DebugGuizmo(dataTable_[selectHCamera_]->GetCamera3D().GetViewMatrix(), dataTable_[selectHCamera_]->GetCamera3D().GetProjectionMatrix());
		}

#endif
	}
}

/// @brief デバッグ用パラメータ
void Engine::Camera3DStore::DebugParameter()
{
#ifdef DEVELOPMENT
	// メニューバーを使用する
	if (!ImGui::Begin("Camera3D"))
	{
		ImGui::End();
		return;
	}

	for (auto& data : dataTable_)data->DebugParameter();

	ImGui::End();
#endif
}


/// @brief デバッグ用レイピッキング
/// @param ray 
/// @param pickList 
void Engine::Camera3DStore::DebugRayPicking(const Collision3D::Ray& ray, std::vector<std::pair<float, DebugData::DebugGuizmoData*>>& pickList)
{
#ifdef DEVELOPMENT

	for (auto& data : dataTable_)
	{
		// デバッグカメラ使用中
		if (debugCamera_->IsEnable())
		{
			data->DebugRayPicking(ray, pickList);
		}
		else
		{
			// デバッグカメラ未使用中

			// 使用中のメインカメラは選択させない
			if (data->GetHandle() == selectHCamera_)
				continue;

			data->DebugRayPicking(ray, pickList);
		}
	}

#endif
}


/// @brief 初期読み込み
/// @param name 
/// @return 
Camera3DHandle Engine::Camera3DStore::InitialLoad(const std::string& name)
{
	// ハンドルの値
	Camera3DHandle hCamera = static_cast<Camera3DHandle>(dataTable_.size());

	// 名前テーブルに記録する
	nameTable_[name] = hCamera;

	// カメラリソースの生成
	std::unique_ptr<Camera3DResource> data = std::make_unique<Camera3DResource>(name, hCamera, parameter_.get());
	dataTable_.push_back(std::move(data));

	return hCamera;
}