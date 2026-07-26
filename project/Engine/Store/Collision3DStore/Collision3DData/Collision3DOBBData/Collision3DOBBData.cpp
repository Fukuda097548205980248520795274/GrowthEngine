#include "Collision3DOBBData.h"
#include "EngineObject/Collision3DInstance/Collision3DInstanceOBB/Collision3DInstanceOBB.h"
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param func 
Engine::Collision3DOBBData::Collision3DOBBData(const std::string& name, Collision3D::Type type, Collision3DHandle hCollision)
	: Collision3DBaseData(name, type, hCollision)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// パラメータの生成と初期化
	param_ = std::make_unique<Collision3D::OBB>();
	param_->center = Vector3(0.0f, 0.0f, 0.0f);
	param_->radius = Vector3(1.0f, 1.0f, 1.0f);
	param_->oriented[0] = Vector3(1.0f, 0.0f, 0.0f);
	param_->oriented[1] = Vector3(0.0f, 1.0f, 0.0f);
	param_->oriented[2] = Vector3(0.0f, 0.0f, 1.0f);
}

/// @brief インスタンスを作成する
/// @return 
void* Engine::Collision3DOBBData::CreateInstance()
{
	std::unique_ptr<Collision3DInstanceOBB> instance = std::make_unique<Collision3DInstanceOBB>(param_.get());
	Collision3DInstanceOBB* pInstance = instance.get();

	instanceTable_.push_back(std::move(instance));

	return pInstance;
}

/// @brief デバッグ用の線を描画する
void Engine::Collision3DOBBData::DebugDrawLine()
{
	for (auto& instance : instanceTable_)
	{
		// OBBに型変換
		Collision3DInstanceOBB* aabbInstance = static_cast<Collision3DInstanceOBB*>(instance.get());

		// ワールド行列
		Matrix4x4 worldMatrix;

		worldMatrix.m[0][0] = aabbInstance->param_->oriented[0].x;
		worldMatrix.m[0][1] = aabbInstance->param_->oriented[0].y;
		worldMatrix.m[0][2] = aabbInstance->param_->oriented[0].z;
		worldMatrix.m[0][3] = 0.0f;

		worldMatrix.m[1][0] = aabbInstance->param_->oriented[1].x;
		worldMatrix.m[1][1] = aabbInstance->param_->oriented[1].y;
		worldMatrix.m[1][2] = aabbInstance->param_->oriented[1].z;
		worldMatrix.m[1][3] = 0.0f;

		worldMatrix.m[2][0] = aabbInstance->param_->oriented[2].x;
		worldMatrix.m[2][1] = aabbInstance->param_->oriented[2].y;
		worldMatrix.m[2][2] = aabbInstance->param_->oriented[2].z;
		worldMatrix.m[2][3] = 0.0f;

		worldMatrix.m[3][0] = aabbInstance->param_->center.x;
		worldMatrix.m[3][1] = aabbInstance->param_->center.y;
		worldMatrix.m[3][2] = aabbInstance->param_->center.z;
		worldMatrix.m[3][3] = 1.0f;


		// ローカル座標
		Vector3 local[8];
		local[0] = { -aabbInstance->param_->radius.x , -aabbInstance->param_->radius.y , -aabbInstance->param_->radius.z };
		local[1] = { aabbInstance->param_->radius.x , -aabbInstance->param_->radius.y , -aabbInstance->param_->radius.z };
		local[2] = { -aabbInstance->param_->radius.x , -aabbInstance->param_->radius.y , aabbInstance->param_->radius.z };
		local[3] = { aabbInstance->param_->radius.x , -aabbInstance->param_->radius.y , aabbInstance->param_->radius.z };
		local[4] = { -aabbInstance->param_->radius.x , aabbInstance->param_->radius.y , -aabbInstance->param_->radius.z };
		local[5] = { aabbInstance->param_->radius.x , aabbInstance->param_->radius.y , -aabbInstance->param_->radius.z };
		local[6] = { -aabbInstance->param_->radius.x , aabbInstance->param_->radius.y , aabbInstance->param_->radius.z };
		local[7] = { aabbInstance->param_->radius.x , aabbInstance->param_->radius.y , aabbInstance->param_->radius.z };

		// 位置
		Vector3 position[8];

		for (uint32_t i = 0; i < 8; i++)
		{
			// ワールド座標
			position[i] = Transform(local[i], worldMatrix);
		}

		// 色
		Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		if (aabbInstance->isCollision_)
			color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

		engine_->DrawDebugLine3D(position[0], position[1], color);
		engine_->DrawDebugLine3D(position[0], position[2], color);
		engine_->DrawDebugLine3D(position[1], position[3], color);
		engine_->DrawDebugLine3D(position[2], position[3], color);

		engine_->DrawDebugLine3D(position[4], position[5], color);
		engine_->DrawDebugLine3D(position[4], position[6], color);
		engine_->DrawDebugLine3D(position[5], position[7], color);
		engine_->DrawDebugLine3D(position[6], position[7], color);

		engine_->DrawDebugLine3D(position[0], position[4], color);
		engine_->DrawDebugLine3D(position[1], position[5], color);
		engine_->DrawDebugLine3D(position[2], position[6], color);
		engine_->DrawDebugLine3D(position[3], position[7], color);
	}
}