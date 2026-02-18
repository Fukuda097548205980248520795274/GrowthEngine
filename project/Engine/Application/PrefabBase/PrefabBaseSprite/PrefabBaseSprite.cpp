#include "PrefabBaseSprite.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param hTexture 
/// @param numInstance 
/// @param name 
PrefabBaseSprite::PrefabBaseSprite(TextureHandle hTexture, uint32_t numInstance, const std::string& name)
	: numInstance_(numInstance), name_(name)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 読み込む
	handle_ = engine_->LoadPrefabSprite(name_, numInstance, hTexture);

	// パラメータを取得する
	param_ = engine_->GetPrefabSpriteParam(handle_);
}

/// @brief インスタンスを生成する
PrefabInstanceSprite* PrefabBaseSprite::CreateInstance()
{
	std::unique_ptr<PrefabInstanceSprite> instance = std::make_unique<PrefabInstanceSprite>(engine_->GetSpriteDrawCall(handle_), param_);
	PrefabInstanceSprite* pInstance = instance.get();

	// テーブルに追加
	instanceTable_.push_back(std::move(instance));

	return pInstance;
}

/// @brief 描画処理
void PrefabBaseSprite::Update()
{
	// 削除したインスタンスを消す
	instanceTable_.remove_if([](std::unique_ptr<PrefabInstanceSprite>& instance) { if (instance->IsDelete()) { return true; }return false; });
}

/// @brief 描画処理
void PrefabBaseSprite::Draw()
{
	engine_->DrawPrefabSprite(handle_);
}