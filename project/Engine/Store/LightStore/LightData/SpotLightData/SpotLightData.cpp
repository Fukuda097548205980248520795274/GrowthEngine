#include "SpotLightData.h"
#include "Parameter/LightParameter/LightParameter.h"
#include "Func/CollisionFunc/CollisionFunc.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
/// @param handle 
Engine::SpotLightData::SpotLightData(const std::string& name, LightHandle handle) : BaseLightData(name, handle)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<Light::SpotLightParam>();
	param_->position = Vector3(0.0f, 0.0f, 0.0f);
	param_->color = Vector3(1.0f, 1.0f, 1.0f);
	param_->direction = Vector3(0.0f, 1.0f, 0.0f);
	param_->intensity = 12.0f;
	param_->distance = 5.0f;
	param_->decay = 4.0f;
	param_->cosAngle = 0.3f;
	param_->cosFalloffStart = 1.0f;
}

/// @brief 更新処理
void Engine::SpotLightData::Update()
{
	// 方向を正規化
	param_->direction = param_->direction.Normalize();
}

/// @brief リセット
void Engine::SpotLightData::Reset()
{
	// 読み込む
	isLoad_ = true;
}

/// @brief デバッグ用の線を描画する
void Engine::SpotLightData::DebugDrawLine()
{
	// ロードしていなかったら何もしない
	if (!isLoad_)return;

	Quaternion q = ToQuaternion(0.0f, param_->direction).Normalize();

	// 回転行列
	Matrix4x4 rotateMatrix = Make3DRotateMatrix4x4(q);

	// ワールド行列
	Matrix4x4 worldMatrix;

	worldMatrix.m[0][0] = rotateMatrix.m[0][0];
	worldMatrix.m[0][1] = rotateMatrix.m[0][1];
	worldMatrix.m[0][2] = rotateMatrix.m[0][2];
	worldMatrix.m[0][3] = 0.0f;

	worldMatrix.m[1][0] = rotateMatrix.m[1][0];
	worldMatrix.m[1][1] = rotateMatrix.m[1][1];
	worldMatrix.m[1][2] = rotateMatrix.m[1][2];
	worldMatrix.m[1][3] = 0.0f;

	worldMatrix.m[2][0] = rotateMatrix.m[2][0];
	worldMatrix.m[2][1] = rotateMatrix.m[2][1];
	worldMatrix.m[2][2] = rotateMatrix.m[2][2];
	worldMatrix.m[2][3] = 0.0f;

	worldMatrix.m[3][0] = param_->position.x;
	worldMatrix.m[3][1] = param_->position.y;
	worldMatrix.m[3][2] = param_->position.z;
	worldMatrix.m[3][3] = 1.0f;


	// ローカル座標
	Vector3 local[8];
	local[0] = { -0.25f , -0.25f , -0.25f };
	local[1] = { 0.25f , -0.25f , -0.25f };
	local[2] = { -0.25f , -0.25f , 0.25f };
	local[3] = { 0.25f , -0.25f , 0.25f };
	local[4] = { -0.25f , 0.25f , -0.25f };
	local[5] = { 0.25f , 0.25f , -0.25f };
	local[6] = { -0.25f , 0.25f , 0.25f };
	local[7] = { 0.25f , 0.25f , 0.25f };

	// 位置
	Vector3 position[8];

	for (uint32_t i = 0; i < 8; i++)
	{
		// ワールド座標
		position[i] = Transform(local[i], worldMatrix);
	}

	engine_->DrawDebugLine3D(position[0], position[1], Vector4(1.0f, 1.0f, 0.0f, 1.0f));
	engine_->DrawDebugLine3D(position[0], position[2], Vector4(1.0f, 1.0f, 0.0f, 1.0f));
	engine_->DrawDebugLine3D(position[1], position[3], Vector4(1.0f, 1.0f, 0.0f, 1.0f));
	engine_->DrawDebugLine3D(position[2], position[3], Vector4(1.0f, 1.0f, 0.0f, 1.0f));

	engine_->DrawDebugLine3D(position[4], position[5], Vector4(1.0f, 1.0f, 0.0f, 1.0f));
	engine_->DrawDebugLine3D(position[4], position[6], Vector4(1.0f, 1.0f, 0.0f, 1.0f));
	engine_->DrawDebugLine3D(position[5], position[7], Vector4(1.0f, 1.0f, 0.0f, 1.0f));
	engine_->DrawDebugLine3D(position[6], position[7], Vector4(1.0f, 1.0f, 0.0f, 1.0f));

	engine_->DrawDebugLine3D(position[0], position[4], Vector4(1.0f, 1.0f, 0.0f, 1.0f));
	engine_->DrawDebugLine3D(position[1], position[5], Vector4(1.0f, 1.0f, 0.0f, 1.0f));
	engine_->DrawDebugLine3D(position[2], position[6], Vector4(1.0f, 1.0f, 0.0f, 1.0f));
	engine_->DrawDebugLine3D(position[3], position[7], Vector4(1.0f, 1.0f, 0.0f, 1.0f));
}

/// @brief デバッグ用レイピッキング
/// @param ray 
/// @param pickList 
void Engine::SpotLightData::DebugRayPicking(const Collision3D::Ray& ray, std::vector<std::pair<float, DebugData::DebugGuizmoData*>>& pickList)
{
	// 選択初期化
	if (debugGuizmoData_.isSelect)
	{
		debugGuizmoData_.isSelect = false;
		return;
	}

	// ロードしていなかったら何もしない
	if (!isLoad_)return;

	Collision3D::AABB aabb;
	aabb.center = param_->position;
	aabb.radius = Vector3(0.25f, 0.25f, 0.25f);

	if (CollisionCheckFunc(aabb, ray))
	{
		std::pair<float, DebugData::DebugGuizmoData*> pick;
		pick.first = Vector3(aabb.center - ray.start).Length();
		pick.second = &debugGuizmoData_;
		pickList.push_back(pick);
	}
}

/// @brief デバッグ用Guizmo操作
void Engine::SpotLightData::DebugGuizmo(const Matrix4x4& viewMatrix, const Matrix4x4& projMatrix)
{
	// ロードしていなかったら何もしない
	if (!isLoad_)return;

	// Gizmoが選択されていなかったら何もしない
	if (!debugGuizmoData_.isSelect)return;

	// ワールド行列
	Matrix4x4 worldMatrix = Make3DTranslateMatrix4x4(param_->position);

	// Gizmo描画
	ImGuizmo::Manipulate(
		&viewMatrix.m[0][0],
		&projMatrix.m[0][0],
		ImGuizmo::TRANSLATE,
		ImGuizmo::LOCAL,
		&worldMatrix.m[0][0]
	);

	// Gizmo を動かしている間だけ、結果を自分の行列系に戻す
	if (ImGuizmo::IsUsing())
	{
		// 平行移動
		param_->position = Vector3(worldMatrix.m[3][0], worldMatrix.m[3][1], worldMatrix.m[3][2]);
	}
}