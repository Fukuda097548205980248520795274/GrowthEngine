#include "PostEffectRadialBlurData.h"
#include "GrowthEngine.h"
#include "PSO/PSOPostEffect/BasePSOPostEffect/BasePSOPostEffect.h"
#include "Resource/OffscreenResource/OffscreenResource.h"

#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

#include "Parameter/PostEffectParameter/PostEffectParameter.h"

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::PostEffectRadialBlurData::Initialize(ID3D12Device* device, Log* log, BasePSOPostEffect* pso)
{
	// nullptrチェック
	assert(device);
	assert(pso);

	// 引数を受け取る
	pso_ = pso;

	// パラメータの生成
	param_ = std::make_unique<PostEffect::RadialBlur>();
	param_->center = Vector2(0.5f, 0.5f);
	param_->sampleCount = 32;
	param_->blur = 1.0f;
	param_->saturation = 1.0f;
	param_->contrast = 1.0f;
	param_->brightness = 0.0f;

	// パラメータを記録する
	group_ = "RadialBlur_" + name_;
	if (parameter_)
	{
		parameter_->SetValue(group_, "Center", &param_->center);
		parameter_->SetValue(group_, "SampleCount", &param_->sampleCount);
		parameter_->SetValue(group_, "Blur", &param_->blur);
		parameter_->SetValue(group_, "Saturation", &param_->saturation);
		parameter_->SetValue(group_, "Contrast", &param_->contrast);
		parameter_->SetValue(group_, "Brightness", &param_->brightness);

		parameter_->RegisterGroupDataReflection(group_);
	}

	// リソース生成
	resource_ = std::make_unique<ConstantBufferResource<PostEffect::RadialBlurDataForGPU>>();
	resource_->Initialize(device, log);
	resource_->data_->resolution =
		Vector2(static_cast<float>(GrowthEngine::GetInstance()->GetScreenWidth()), static_cast<float>(GrowthEngine::GetInstance()->GetScreenHeight()));
	resource_->data_->sampleCount = param_->sampleCount;
	resource_->data_->blur = param_->blur;
	resource_->data_->saturation = param_->saturation;
	resource_->data_->contrast = param_->contrast;
	resource_->data_->brightness = param_->brightness;
	resource_->data_->center = param_->center;
}

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::PostEffectRadialBlurData::Register(ID3D12GraphicsCommandList* commandList, OffscreenResource* offscreenResource)
{
	/*-----------------
	    データを渡す
	-----------------*/

	resource_->data_->sampleCount = param_->sampleCount;
	resource_->data_->blur = param_->blur;
	resource_->data_->saturation = param_->saturation;
	resource_->data_->contrast = param_->contrast;
	resource_->data_->brightness = param_->brightness;
	resource_->data_->center = param_->center;


	/*------------------------
	    コマンドリストに登録
	------------------------*/

	// PSOの設定
	pso_->Register(commandList);

	// テクスチャの設定
	offscreenResource->Register(commandList, 0);

	// パラメータの設定
	resource_->RegisterGraphics(commandList, 1);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawInstanced(3, 1, 0, 0);

}

/// @brief デバッグ用パラメータ
void Engine::PostEffectRadialBlurData::DebugParameter()
{
#ifdef _DEVELOPMENT

	// UV
	if (ImGui::TreeNode((name_ + "_RadialBlur").c_str()))
	{
		ImGui::DragFloat("Blur", &param_->blur, 0.1f, -1000.0f, 1000.0f);
		ImGui::DragFloat2("Center", &param_->center.x, 0.01f, -10.0f, 10.0f);
		ImGui::DragInt("SampleCount", &param_->sampleCount, 1, 0);
		ImGui::DragFloat("Saturation", &param_->saturation, 0.1f, -1000.0f, 1000.0f);
		ImGui::DragFloat("Contrast", &param_->contrast, 0.1f, 0.0f, 1000.0f);
		ImGui::DragFloat("Brightness", &param_->brightness, 0.1f, 0.0f, 1000.0f);

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

		// 終了
		ImGui::TreePop();
	}

#endif
}