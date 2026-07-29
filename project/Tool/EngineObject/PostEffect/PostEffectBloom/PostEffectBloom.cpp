#include "PostEffectBloom.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 名前
PostEffectBloom::PostEffectBloom(const std::string& name) : Engine::BasePostEffect(name)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 種類を決める
	type_ = Engine::PostEffect::Type::Bloom;

	// 読み込む
	handle_ = engine_->LoadPostEffect(name_, type_);

	// パラメータを取得する
	param_ = engine_->GetPostEffectParam<Engine::PostEffect::Bloom>(handle_);
}

/// @brief 描画処理
void PostEffectBloom::Draw()
{
	// 描画
	engine_->DrawPostEffect(handle_);
}