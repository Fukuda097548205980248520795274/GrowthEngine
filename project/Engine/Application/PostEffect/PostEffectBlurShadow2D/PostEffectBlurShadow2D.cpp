#include "PostEffectBlurShadow2D.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 名前
PostEffectBlurShadow2D::PostEffectBlurShadow2D(const std::string& name) : BasePostEffect(name)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 種類を決める
	type_ = Engine::PostEffect::Type::BlurShadow2D;

	// 読み込む
	handle_ = engine_->LoadPostEffect(name_, type_);

	// パラメータを取得する
	param_ = engine_->GetPostEffectParam<Engine::PostEffect::BlurShadow2D>(handle_);
}

/// @brief 描画処理
void PostEffectBlurShadow2D::Draw()
{
	// 描画
	engine_->DrawPostEffect(handle_);
}