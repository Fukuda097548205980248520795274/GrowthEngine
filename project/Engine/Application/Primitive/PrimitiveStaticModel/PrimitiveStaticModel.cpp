#include "PrimitiveStaticModel.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param modelHandle 
/// @param name 
PrimitiveStaticModel::PrimitiveStaticModel(ModelHandle modelHandle, const std::string& name)
	: modelHandle_(modelHandle) , name_(name)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();
}

/// @brief 描画処理
void PrimitiveStaticModel::Draw()
{

}