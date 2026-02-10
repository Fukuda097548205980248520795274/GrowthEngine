#include "DirectionalLightData.h"
#include <cassert>
#include <cmath>
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
/// @param handle 
Engine::DirectionalLightData::DirectionalLightData(const std::string& name, LightHandle handle) 
	: BaseLightData(name, handle) 
{
	// パラメータ生成
	param_ = std::make_unique<DirectionalLightParam>();
	param_->direction = Vector3(0.0f, -1.0f, 0.0f);
	param_->intensity = 1.0f;
	param_->color = Vector3(1.0f, 1.0f, 1.0f);
	param_->position = Vector3(0.0f, 0.0f, 0.0f);
	param_->size = Vector2(20.0f, 20.0f);
	param_->minDepth = 0.1f;
	param_->maxDepth = 15.0f;
}

/// @brief 初期化
/// @param heap 
/// @param device 
/// @param log 
void Engine::DirectionalLightData::Initialize(DX12Heap* heap, ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(heap);
	assert(device);

	// エンジンのインスタンスを取得する
	const GrowthEngine* engine = GrowthEngine::GetInstance();

	// シャドウマップ用テクスチャリソースの生成と初期化
	shadowMapTextureResource_ = std::make_unique<ShadowMapTextureResource>();
	shadowMapTextureResource_->Initialize(heap, device, engine->GetScreenWidth(), engine->GetScreenHeight(), log);
}

/// @brief ビュープロジェクション行列を取得する
/// @return 
Matrix4x4 Engine::DirectionalLightData::GetViewProjectionMatrix() const
{
	Vector3 rotate{};
	rotate.y = std::atan2(param_->direction.x, param_->direction.z);
	float length = std::sqrt(std::pow(param_->direction.x, 2.0f) + std::pow(param_->direction.z, 2.0f));
	rotate.x = std::atan2(-param_->direction.y, length);
	rotate.z = 0.0f;

	// ビュー行列
	Matrix4x4 view = (Make3DRotateMatrix4x4(rotate) * Make3DTranslateMatrix4x4(param_->position)).Inverse();

	// 平行投影行列
	Matrix4x4 projection = MakeOrthographicMatrix4x4(-param_->size.x / 2.0f, param_->size.y / 2.0f, param_->size.x / 2.0f, -param_->size.y / 2.0f,
		param_->minDepth, param_->maxDepth);

	return view * projection;
}