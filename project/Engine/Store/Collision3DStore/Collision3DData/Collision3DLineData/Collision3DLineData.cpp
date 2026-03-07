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
	std::unique_ptr<Collision3DInstanceLine> instance = std::make_unique<Collision3DInstanceLine>(param_.get());
	Collision3DInstanceLine* pInstance = instance.get();

	instanceTable_.push_back(std::move(instance));

	return pInstance;
}