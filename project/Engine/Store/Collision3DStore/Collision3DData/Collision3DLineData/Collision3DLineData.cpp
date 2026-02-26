#include "Collision3DLineData.h"
#include "Application/Collision3DInstance/Collision3DInstanceLine/Collision3DInstanceLine.h"

/// @brief コンストラクタ
/// @param func 
Engine::Collision3DLineData::Collision3DLineData(std::function<void()> func, const std::string& name, Collision3D::Type type, Collision3DHandle hCollision)
	: Collision3DBaseData(func, name, type, hCollision)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<Collision3D::Line>();
	param_->start = Vector3(0.0f, -1.0f, 0.0f);
	param_->diff = Vector3(0.0f, 2.0f, 0.0f);
}

/// @brief インスタンスを作成する
/// @return 
void* Engine::Collision3DLineData::CreateInstance()
{
	// パラメータの生成
	std::unique_ptr<Collision3D::Line> instanceParam = std::make_unique<Collision3D::Line>();
	instanceParam->start = param_->start;
	instanceParam->diff = param_->diff;

	std::unique_ptr<Collision3DInstanceLine> instance = std::make_unique<Collision3DInstanceLine>(instanceParam.get());
	Collision3DInstanceLine* pInstance = instance.get();

	instanceParams_.push_back(std::move(instanceParam));
	instanceTable_.push_back(std::move(instance));

	return pInstance;
}