#include "PointLightData.h"
#include "Parameter/LightParameter/LightParameter.h"
#include "GrowthEngine.h"
#include "Func/CollisionFunc/CollisionFunc.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

/// @brief コンストラクタ
/// @param name 
/// @param handle 
Engine::PointLightData::PointLightData(const std::string& name, LightHandle handle) : BaseLightData(name,handle)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<Light::PointLightParam>();
	param_->position = Vector3(0.0f, 0.0f, 0.0f);
	param_->color = Vector3(1.0f, 1.0f, 1.0f);
	param_->intensity = 12.0f;
	param_->radius = 5.0f;
	param_->decay = 4.0f;

	group_ = "Point_" + name_;
}

/// @brief 初期化
/// @param parameter 
void Engine::PointLightData::Initialize(LightParameter* parameter)
{
	// 基底クラス初期化
	BaseLightData::Initialize(parameter);

	parameter_->SetValue(group_, "Intensity", &param_->intensity);
	parameter_->SetValue(group_, "Color", &param_->color);
	parameter_->SetValue(group_, "Position", &param_->position);
	parameter_->SetValue(group_, "Radius", &param_->radius);
	parameter_->SetValue(group_, "Decay", &param_->decay);
	parameter_->RegisterGroupDataReflection(group_);
}

/// @brief 更新処理
void Engine::PointLightData::Update()
{

}

/// @brief リセット
void Engine::PointLightData::Reset()
{
	// ファイルがあった時
	if (parameter_->IsFileFound(group_))
	{
		parameter_->RegisterGroupDataReflection(group_);
	}
	else
	{
		param_->position = Vector3(0.0f, 0.0f, 0.0f);
		param_->color = Vector3(1.0f, 1.0f, 1.0f);
		param_->intensity = 12.0f;
		param_->radius = 5.0f;
		param_->decay = 4.0f;
	}

	// 読み込む
	isLoad_ = true;
}

/// @brief デバッグ用描画処理
void Engine::PointLightData::DebugParameter()
{
#ifdef DEVELOPMENT

	// 読み込んでいないときは使えない
	if (!isLoad_)return;

	if (ImGui::TreeNode((name_).c_str()))
	{
		// 位置
		ImGui::DragFloat3("Position", &param_->position.x, 0.1f, -100000.0f, 100000.0f);

		// 輝度
		ImGui::DragFloat("Intensity", &param_->intensity, 0.01f);

		// 色
		ImGui::ColorEdit3("Color", &param_->color.x);

		// ライトが届く距離
		ImGui::DragFloat("Radius", &param_->radius, 0.1f);

		// 減衰率
		ImGui::DragFloat("Decay", &param_->decay, 0.01f);


		ImGui::Text("\n");

		// 保存ボタン
		if (ImGui::Button("Save"))
		{
			parameter_->SaveFile(group_);
			std::string message = std::format("{} : saved.", group_);
			MessageBoxA(nullptr, message.c_str(), "RecordSetting", 0);
		}

		ImGui::Text("\n");

		// ロードボタン
		if (ImGui::Button("Load"))
		{
			parameter_->RegisterGroupDataReflection(group_);
			std::string message = std::format("{} : loaded.", group_);
			MessageBoxA(nullptr, message.c_str(), "RecordSetting", 0);
		}

		ImGui::Text("\n");


		ImGui::TreePop();
	}

#endif
}

/// @brief デバッグ用の線を描画する
void Engine::PointLightData::DebugDrawLine()
{
	// ロードしていなかったら何もしない
	if (!isLoad_)return;

	// ワールド行列
	Matrix4x4 worldMatrix;

	worldMatrix.m[0][0] = 1.0f;
	worldMatrix.m[0][1] = 0.0f;
	worldMatrix.m[0][2] = 0.0f;
	worldMatrix.m[0][3] = 0.0f;

	worldMatrix.m[1][0] = 0.0f;
	worldMatrix.m[1][1] = 1.0f;
	worldMatrix.m[1][2] = 0.0f;
	worldMatrix.m[1][3] = 0.0f;

	worldMatrix.m[2][0] = 0.0f;
	worldMatrix.m[2][1] = 0.0f;
	worldMatrix.m[2][2] = 1.0f;
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
void Engine::PointLightData::DebugRayPicking(const Collision3D::Ray& ray, std::vector<std::pair<float, DebugData::DebugGuizmoData*>>& pickList)
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
void Engine::PointLightData::DebugGuizmo(const Matrix4x4& viewMatrix, const Matrix4x4& projMatrix)
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