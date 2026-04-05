#include "Camera3DResource.h"
#include "GrowthEngine.h"
#include <numbers>
#include "Func/CollisionFunc/CollisionFunc.h"
#include "Parameter/Camera3DParameter/Camera3DParameter.h"

/// @brief コンストラクタ
/// @param name 
/// @param hCamera 
Engine::Camera3DResource::Camera3DResource(const std::string& name, Camera3DHandle hCamera, Camera3DParameter* parameter)
	: name_(name), hCamera_(hCamera) , parameter_(parameter)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// カメラの生成と初期化
	camera3d_ = std::make_unique<Camera3D>();

	// パラメータを取得する
	param_ = camera3d_->GetParam();

	// パラメータを記録する
	if (parameter_)
	{
		parameter_->SetValue(name_, "Transform_Rotate", &param_->transform.rotate);
		parameter_->SetValue(name_, "Transform_Translate", &param_->transform.translate);
		parameter_->SetValue(name_, "FOV", &param_->setting.fov);
		parameter_->SetValue(name_, "NearClip", &param_->setting.nearClip);
		parameter_->SetValue(name_, "FarClip", &param_->setting.nearClip);
		parameter_->RegisterGroupDataReflection(name_);
	}

	// 読み込む
	isLoad_ = true;
}

/// @brief 更新処理
void Engine::Camera3DResource::Update()
{
	// カメラの更新
	camera3d_->Update();
}

/// @brief リセット
void Engine::Camera3DResource::Reset()
{
	if (parameter_->IsFileFound(name_))
	{
		parameter_->RegisterGroupDataReflection(name_);
	}
	else
	{
		// トランスフォーム
		param_->transform.rotate = Vector3(0.0f, 0.0f, 0.0f);
		param_->transform.translate = Vector3(0.0f, 0.0f, 0.0f);

		// 設定
		param_->setting.fov = 0.45f;
		param_->setting.nearClip = 0.01f;
		param_->setting.farClip = 800.0f;
	}

	// ロードされたとみなす
	isLoad_ = true;
}

/// @brief シーン前のリセット
void Engine::Camera3DResource::PerSceneReset()
{
	// 読み込みをリセットする
	isLoad_ = false;
}

/// @brief デバッグ用の線を描画する
void Engine::Camera3DResource::DebugDrawLine(const Vector4& color)
{
	// ロードされていないときは表示しない
	if (!isLoad_)return;

	// 回転行列
	Matrix4x4 rotateMatrix = Make3DRotateMatrix4x4(camera3d_->GetQuaternion());

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

	worldMatrix.m[3][0] = param_->transform.translate.x;
	worldMatrix.m[3][1] = param_->transform.translate.y;
	worldMatrix.m[3][2] = param_->transform.translate.z;
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

/// @brief デバッグ用パラメータ
void Engine::Camera3DResource::DebugParameter()
{
	// ロードされていないときは表示しない
	if (isLoad_ == false)
		return;

	if (ImGui::TreeNode(name_.c_str()))
	{
		// トランスフォーム
		if (ImGui::TreeNode("Transform"))
		{
			ImGui::DragFloat3("Rotate", &param_->transform.rotate.x, 0.01f);
			ImGui::DragFloat3("Translate", &param_->transform.translate.x, 0.01f);

			ImGui::TreePop();
		}

		// 設定
		if (ImGui::TreeNode(name_.c_str()))
		{
			ImGui::DragFloat("FOV", &param_->setting.fov, 0.01f);
			ImGui::DragFloat("NearClip", &param_->setting.nearClip, 0.01f);
			ImGui::DragFloat("FarClip", &param_->setting.farClip, 0.1f);

			ImGui::TreePop();
		}

		ImGui::Text("\n");

		// 保存ボタン
		if (ImGui::Button("Save"))
		{
			parameter_->SaveFile(name_);
			std::string message = std::format("{} : saved.", name_);
			MessageBoxA(nullptr, message.c_str(), "RecordSetting", 0);
		}

		ImGui::Text("\n");

		// ロードボタン
		if (ImGui::Button("Load"))
		{
			parameter_->RegisterGroupDataReflection(name_);
			std::string message = std::format("{} : loaded.", name_);
			MessageBoxA(nullptr, message.c_str(), "RecordSetting", 0);
		}

		ImGui::Text("\n");

		ImGui::TreePop();
	}
}

/// @brief デバッグ用レイピッキング
/// @param ray 
/// @param pickList 
void Engine::Camera3DResource::DebugRayPicking(const Collision3D::Ray& ray, std::vector<std::pair<float, DebugData::DebugGuizmoData*>>& pickList)
{
	// 選択初期化
	if (guizmoData_.isSelect)
	{
		guizmoData_.isSelect = false;
		return;
	}

	// ロードされていないときは判定しない
	if (!isLoad_)return;

	Collision3D::AABB aabb;
	aabb.center = param_->transform.translate;
	aabb.radius = Vector3(0.25f, 0.25f, 0.25f);

	if (CollisionCheckFunc(aabb, ray))
	{
		std::pair<float, DebugData::DebugGuizmoData*> pick;
		pick.first = Vector3(aabb.center - ray.start).Length();
		pick.second = &guizmoData_;
		pickList.push_back(pick);
	}
}

/// @brief Guizmo操作
/// @param viewMatrix 
/// @param projMatrix 
void Engine::Camera3DResource::DebugGuizmo(const Matrix4x4& viewMatrix, const Matrix4x4& projMatrix)
{
	// 読み込んでいないと処理しない
	if (!isLoad_)return;

	// 選択していないときは処理しない
	if (!guizmoData_.isSelect)
		return;

	// Tキー -> 移動
	if (engine_->GetKeyTrigger(DIK_T))guizmoData_.mode = DebugData::GuizmoMode::Translate;

	// Rキー -> 回転
	if (engine_->GetKeyTrigger(DIK_R))guizmoData_.mode = DebugData::GuizmoMode::Rotate;

	// 現在の回転・移動からワールド行列を一度だけ生成する
	Quaternion rotateQ =
		ToQuaternion(param_->transform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	// ワールド行列
	Matrix4x4 worldMatrix =
		Make3DRotateMatrix4x4(rotateQ) *
		Make3DTranslateMatrix4x4(param_->transform.translate);

	// Guizmoの操作タイプを切り替える
	ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
	if (guizmoData_.mode == DebugData::GuizmoMode::Rotate)
	{
		operation = ImGuizmo::ROTATE;
	}

	// 操作モードだけ切り替えて同じ行列を編集する
	ImGuizmo::Manipulate(&viewMatrix.m[0][0], &projMatrix.m[0][0], operation, ImGuizmo::LOCAL, &worldMatrix.m[0][0]);

	if (ImGuizmo::IsUsing())
	{
		float translation[3];
		float rotation[3];
		float scale[3];

		// ワールド行列から回転・移動を分解する
		ImGuizmo::DecomposeMatrixToComponents(&worldMatrix.m[0][0], translation, rotation, scale);

		// 度数法(Degrees)から弧度法(Radians)へ変換するための係数
		constexpr float DEG2RAD = std::numbers::pi_v<float> / 180.0f;

		if (guizmoData_.mode == DebugData::GuizmoMode::Translate)
		{
			param_->transform.translate = Vector3(translation[0], translation[1], translation[2]);
		}
		else
		{
			param_->transform.rotate.x = rotation[0] * DEG2RAD;
			param_->transform.rotate.y = rotation[1] * DEG2RAD;
			param_->transform.rotate.z = rotation[2] * DEG2RAD;
		}
	}
}