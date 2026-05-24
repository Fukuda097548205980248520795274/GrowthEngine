#include "EffectManager.h"

/// @brief インスタンス
std::unique_ptr<EffectManager> EffectManager::instance_ = nullptr;

/// @brief インスタンスを取得する
/// @return 
EffectManager* EffectManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_.reset(new EffectManager());
		instance_->Initialize();
	}

	return instance_.get();
}


/// @brief 初期化処理
void EffectManager::Initialize()
{

}

/// @brief 更新処理
void EffectManager::Update()
{

}

/// @brief 描画処理
void EffectManager::Draw()
{

}