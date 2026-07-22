#include "PostEffectVignettingData.h"
#include "GrowthEngine.h"
#include "Resource/OffscreenResource/OffscreenResource.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include "Parameter/PostEffectParameter/PostEffectParameter.h"

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::PostEffectVignettingData::Initialize(ID3D12Device* device, Log* log, BasePSOPostEffect* pso)
{
	// nullptrチェック
	assert(device);
	assert(pso);

	// 引数を受け取る
	pso_ = pso;

	// パラメータの生成
	param_ = std::make_unique<PostEffect::Vignetting>();
	param_->color = Vector3(0.0f, 0.0f, 0.0f);
	param_->intensity = 1.0f;
	param_->power = 0.8f;

	// パラメータを記録する
	group_ = "Vignetting_" + name_;
	if (parameter_)
	{
		parameter_->SetValue(group_, "Color", &param_->color);
		parameter_->SetValue(group_, "Intensity", &param_->intensity);
		parameter_->SetValue(group_, "Power", &param_->power);

		parameter_->RegisterGroupDataReflection(group_);
	}

	// リソース生成
	resource_ = std::make_unique<ConstantBufferResource<PostEffect::VignettingDataForGPU>>();
	resource_->Initialize(device, log);
	resource_->data_->color = param_->color;
	resource_->data_->intensity = param_->intensity;
	resource_->data_->power = param_->power;
}

/// @brief リセット
void Engine::PostEffectVignettingData::Reset()
{
	if (parameter_->IsFileFound(group_))
	{
		parameter_->RegisterGroupDataReflection(group_);
	}
	else
	{
		param_->color = Vector3(0.0f, 0.0f, 0.0f);
		param_->intensity = 1.0f;
		param_->power = 0.8f;
	}
}

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::PostEffectVignettingData::Register(const PostEffectRenderContext& context)
{
    ID3D12GraphicsCommandList* commandList = context.commandList;
	OffscreenResource* offscreenPixelShaderResource = context.offscreenPixelShaderResource;

	/*-----------------
		データを渡す
	-----------------*/

	resource_->data_->color = param_->color;
	resource_->data_->intensity = param_->intensity;
	resource_->data_->power = param_->power;


	/*------------------------
		コマンドリストに登録
	------------------------*/

	// PSOの設定
	pso_->Register(commandList);

	// テクスチャの設定
	offscreenPixelShaderResource->RegisterGraphics(commandList, 0);

	// パラメータの設定
	resource_->RegisterGraphics(commandList, 1);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawInstanced(3, 1, 0, 0);

}

/// @brief デバッグ用パラメータ
void Engine::PostEffectVignettingData::DebugParameter()
{
#ifdef DEVELOPMENT

	// UV
	if (ImGui::TreeNode((name_ + "_Vignetting").c_str()))
	{
		ImGui::ColorEdit3("Color", &param_->color.x);
		ImGui::SliderFloat("Intensity", &param_->intensity, 0.0f, 1.0f);
		ImGui::DragFloat("Power", &param_->power, 0.01f, 0.0f, 50.0f);

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