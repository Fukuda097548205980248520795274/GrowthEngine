#include "Render3DStaticModel.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param modelHandle 
/// @param name 
Render3DStaticModel::Render3DStaticModel(ModelHandle hModel, const std::string& name) : BaseRender3D(name)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 種別名
	type_ = Engine::Render3D::Type::StaticModel;

	// 読み込み
	hRender3D_ = engine_->LoadRender3D(hModel, 0, 0, name_, type_);

	// パラメータを取得する
	param_ = engine_->GetRender3DParam<Engine::Render3D::StaticModel::Param>(hRender3D_);
}

/// @brief 描画処理
void Render3DStaticModel::Draw()
{
	engine_->DrawRender3D(hRender3D_);
}