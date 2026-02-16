#include "Camera3DStore.h"
#include <cassert>

/// @brief コンストラクタ
Engine::Camera3DStore::Camera3DStore()
{
	// 初期カメラを読み込む
	selectHCamera_ = InitialLoad("Initial");

#ifdef _DEVELOPMENT

	// デバッグカメラの生成
	debugCamera_ = std::make_unique<DebugCameraResource>();

#endif
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
			return data->GetHandle();
	}

	// ハンドルの値
	Camera3DHandle hCamera = static_cast<Camera3DHandle>(dataTable_.size());

	// 初めての読み込みカメラは自動で切り替え
	if (dataTable_.size() == 1)selectHCamera_ = hCamera;

	// カメラリソースの生成
	std::unique_ptr<Camera3DResource> data = std::make_unique<Camera3DResource>(name, hCamera);
	dataTable_.push_back(std::move(data));

	return hCamera;
}

/// @brief 更新処理
void Engine::Camera3DStore::Update()
{
	// 指定されたカメラの更新
	dataTable_[selectHCamera_]->Update();

#ifdef _DEVELOPMENT

	// デバッグカメラ更新
	debugCamera_->Update();

#endif
}

/// @brief 3Dカメラデータを取得する
/// @return 
const Engine::Camera3D& Engine::Camera3DStore::GetCamera3D() const
{
#ifdef _DEVELOPMENT

	// デバッグカメラ有効時
	if (debugCamera_->IsEnable())
		return debugCamera_->GetCamera3D();

#endif

	return dataTable_[selectHCamera_]->GetCamera3D(); 
}


/// @brief 初期読み込み
/// @param name 
/// @return 
Camera3DHandle Engine::Camera3DStore::InitialLoad(const std::string& name)
{
	// ハンドルの値
	Camera3DHandle hCamera = static_cast<Camera3DHandle>(dataTable_.size());

	// カメラリソースの生成
	std::unique_ptr<Camera3DResource> data = std::make_unique<Camera3DResource>(name, hCamera);
	dataTable_.push_back(std::move(data));

	return hCamera;
}