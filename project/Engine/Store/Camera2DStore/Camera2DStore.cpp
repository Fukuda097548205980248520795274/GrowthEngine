#include "Camera2DStore.h"
#include <cassert>

/// @brief コンストラクタ
Engine::Camera2DStore::Camera2DStore()
{
	// 初期カメラを読み込む
	selectHCamera_ = InitialLoad("Initial");
}

/// @brief 読み込み
/// @param name 名前
/// @return 
Camera2DHandle Engine::Camera2DStore::Load(const std::string& name)
{
	// 同じデータがないか確認
	for (auto& data : dataTable_)
	{
		if (name == data->GetName())
			return data->GetHandle();
	}

	// ハンドルの値
	Camera2DHandle hCamera = static_cast<Camera2DHandle>(dataTable_.size());

	// 初めての読み込みカメラは自動で切り替え
	if (dataTable_.size() == 1)selectHCamera_ = hCamera;

	// カメラリソースの生成
	std::unique_ptr<Camera2DResource> data = std::make_unique<Camera2DResource>(name, hCamera);
	dataTable_.push_back(std::move(data));

	return hCamera;
}

/// @brief 更新処理
void Engine::Camera2DStore::Update()
{
	// 指定されたカメラの更新
	dataTable_[selectHCamera_]->Update();
}

/// @brief 2Dカメラデータを取得する
/// @return 
const Engine::Camera2D& Engine::Camera2DStore::GetCamera2D() const
{
	return dataTable_[selectHCamera_]->GetCamera2D();
}


/// @brief 初期読み込み
/// @param name 
/// @return 
Camera2DHandle Engine::Camera2DStore::InitialLoad(const std::string& name)
{
	// ハンドルの値
	Camera2DHandle hCamera = static_cast<Camera2DHandle>(dataTable_.size());

	// カメラリソースの生成
	std::unique_ptr<Camera2DResource> data = std::make_unique<Camera2DResource>(name, hCamera);
	dataTable_.push_back(std::move(data));

	return hCamera;
}