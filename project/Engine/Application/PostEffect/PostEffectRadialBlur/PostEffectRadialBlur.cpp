#include "PostEffectRadialBlur.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 名前
PostEffectRadialBlur::PostEffectRadialBlur(std::string name) : name_(name)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 種類を決める
	type_ = Engine::PostEffect::Type::RadialBlur;

	// 読み込む
	handle_ = engine_->LoadPostEffect(name_, type_);
}

/// @brief 描画処理
void PostEffectRadialBlur::Draw()
{
	// 描画
	engine_->DrawPostEffect(handle_);
}