#include "CameraStore.h"
#include <cassert>

/// @brief コンストラクタ
Engine::CameraStore::CameraStore()
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
CameraHandle Engine::CameraStore::Load(GameCamera* gameCamera, const std::string& name)
{
	// nullptrチェック
	assert(gameCamera);

	// 同じデータがないか確認
	for (auto& data : dataTable_)
	{
		if (name == data->GetName())
		{
			data->DataReflect(gameCamera);
			return data->GetHandle();
		}
	}

	// ハンドルの値
	CameraHandle hCamera = static_cast<CameraHandle>(dataTable_.size());

	// 初めての読み込みカメラは自動で切り替え
	if (dataTable_.size() == 1)selectHCamera_ = hCamera;

	// カメラリソースの生成
	std::unique_ptr<CameraResource> data = std::make_unique<CameraResource>(name, hCamera);
	data->DataReflect(gameCamera);
	dataTable_.push_back(std::move(data));

	return hCamera;
}

/// @brief 更新処理
void Engine::CameraStore::Update()
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
const Camera3D& Engine::CameraStore::GetCamera3D() const
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
CameraHandle Engine::CameraStore::InitialLoad(const std::string& name)
{
	// ハンドルの値
	CameraHandle hCamera = static_cast<CameraHandle>(dataTable_.size());

	// カメラリソースの生成
	std::unique_ptr<CameraResource> data = std::make_unique<CameraResource>(name, hCamera);
	dataTable_.push_back(std::move(data));

	return hCamera;
}