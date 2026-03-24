#include "SpotLightData.h"
#include "Parameter/LightParameter/LightParameter.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

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

	group_ = "Spot_" + name_;
}

/// @brief 初期化
/// @param parameter 
void Engine::SpotLightData::Initialize(LightParameter* parameter)
{
	// 基底クラス初期化
	BaseLightData::Initialize(parameter);

	parameter_->SetValue(group_, "Intensity", &param_->intensity);
	parameter_->SetValue(group_, "Color", &param_->color);
	parameter_->SetValue(group_, "Position", &param_->position);
	parameter_->SetValue(group_, "Direction", &param_->direction);
	parameter_->SetValue(group_, "Distance", &param_->distance);
	parameter_->SetValue(group_, "Decay", &param_->decay);
	parameter_->SetValue(group_, "CosAngle", &param_->cosAngle);
	parameter_->SetValue(group_, "CosFalloffStart", &param_->cosFalloffStart);
	parameter_->RegisterGroupDataReflection(group_);
}

/// @brief リセット
void Engine::SpotLightData::Reset()
{
	// ファイルがあった時
	if (parameter_->IsFileFound(group_))
	{
		parameter_->RegisterGroupDataReflection(group_);
	}
	else
	{
		// なかったとき

		param_->position = Vector3(0.0f, 0.0f, 0.0f);
		param_->color = Vector3(1.0f, 1.0f, 1.0f);
		param_->direction = Vector3(0.0f, 1.0f, 0.0f);
		param_->intensity = 12.0f;
		param_->distance = 5.0f;
		param_->decay = 4.0f;
		param_->cosAngle = 0.3f;
		param_->cosFalloffStart = 1.0f;
	}

	// 読み込む
	isLoad_ = true;
}

/// @brief デバッグ用描画処理
void Engine::SpotLightData::DebugParameter()
{
#ifdef _DEVELOPMENT

	// 読み込んでいないときは使えない
	if (!isLoad_)return;

	if (ImGui::TreeNode((name_).c_str()))
	{
		// 位置
		ImGui::DragFloat3("Position", &param_->position.x, 0.1f, -100000.0f, 100000.0f);

		// 方向
		ImGui::DragFloat3("Direction", &param_->direction.x, 0.1f);

		// 輝度
		ImGui::DragFloat("Intensity", &param_->intensity, 0.01f);

		// 色
		ImGui::ColorEdit3("Color", &param_->color.x);

		// ライトが届く距離
		ImGui::DragFloat("Distance", &param_->distance, 0.1f);

		// 減衰率
		ImGui::DragFloat("Decay", &param_->decay, 0.01f);

		// 光の角度
		ImGui::DragFloat("CosAngle", &param_->cosAngle, 0.1f);

		// フォールアウト開始角度
		ImGui::DragFloat("CosFalloutStart", &param_->cosFalloffStart, 0.01f);


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