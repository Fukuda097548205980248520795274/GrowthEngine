#include "PostEffectGrayscaleData.h"
#include "GrowthEngine.h"
#include "PSO/PSOPostEffect/BasePSOPostEffect/BasePSOPostEffect.h"
#include "Resource/OffscreenResource/OffscreenResource.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include "Parameter/PostEffectParameter/PostEffectParameter.h"

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::PostEffectGrayscaleData::Initialize(ID3D12Device* device, Log* log, BasePSOPostEffect* pso)
{
	// nullptrチェック
	assert(device);
	assert(pso);

	// 引数を受け取る
	pso_ = pso;

	// パラメータの生成
	param_ = std::make_unique<PostEffect::Grayscale>();
	param_->colorWeight = Vector3(0.2125f, 0.7154f, 0.0721f);
	param_->intensity = 1.0f;
	param_->tint = Vector3(1.0f, 1.0f, 1.0f);
	param_->contrast = 1.0f;
	param_->brightness = 0.0f;

	// パラメータを記録する
	group_ = "Grayscale_" + name_;
	if (parameter_)
	{
		parameter_->SetValue(group_, "RGB_Weight", &param_->colorWeight);
		parameter_->SetValue(group_, "Intensity", &param_->intensity);
		parameter_->SetValue(group_, "Tint", &param_->tint);
		parameter_->SetValue(group_, "Contrast", &param_->contrast);
		parameter_->SetValue(group_, "Brightness", &param_->brightness);

		parameter_->RegisterGroupDataReflection(group_);
	}

	// リソース生成
	resource_ = std::make_unique<ConstantBufferResource<PostEffect::GrayscaleDataForGPU>>();
	resource_->Initialize(device, log);
	resource_->data_->colorWeight = param_->colorWeight;
	resource_->data_->intensity = param_->intensity;
	resource_->data_->tint = param_->tint;
	resource_->data_->contrast = param_->contrast;
	resource_->data_->brightness = param_->brightness;
}

/// @brief リセット
void Engine::PostEffectGrayscaleData::Reset()
{
	if (parameter_->IsFileFound(group_))
	{
		parameter_->RegisterGroupDataReflection(group_);
	}
	else
	{
		param_->colorWeight = Vector3(0.2125f, 0.7154f, 0.0721f);
		param_->intensity = 1.0f;
		param_->tint = Vector3(1.0f, 1.0f, 1.0f);
		param_->contrast = 1.0f;
		param_->brightness = 0.0f;
	}
}

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::PostEffectGrayscaleData::Register(ID3D12GraphicsCommandList* commandList, OffscreenResource* offscreenResource)
{
	/*-----------------
		データを渡す
	-----------------*/

	resource_->data_->colorWeight = param_->colorWeight;
	resource_->data_->intensity = param_->intensity;
	resource_->data_->tint = param_->tint;
	resource_->data_->contrast = param_->contrast;
	resource_->data_->brightness = param_->brightness;


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
void Engine::PostEffectGrayscaleData::DebugParameter()
{
#ifdef _DEVELOPMENT

	if (ImGui::TreeNode((name_ + "_Grayscale").c_str()))
	{
		ImGui::ColorEdit3("RGB Weight", &param_->colorWeight.x);
		ImGui::SliderFloat("Intensity", &param_->intensity, 0.0f, 1.0f);
		ImGui::ColorEdit3("Tint", &param_->tint.x);
		ImGui::DragFloat("Contrast", &param_->contrast, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat("Brightness", &param_->brightness,0.01f, -1.0f, 1.0f);

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